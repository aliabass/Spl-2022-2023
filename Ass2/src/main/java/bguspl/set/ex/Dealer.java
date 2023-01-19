package bguspl.set.ex;

import bguspl.set.Env;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.logging.Level;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * This class manages the dealer's threads and data
 */
public class Dealer implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;
    private final Player[] players;

    /**
     * The list of card ids that are left in the dealer's deck.
     */
    private final List<Integer> deck;

    /**
     * True iff game should be terminated due to an external event.
     */
    private volatile boolean terminate;


    long startTime;

    long time;

    public BlockingQueue<int[]> sets;

    private List<Integer> RandomSlots;

    Thread[] playersThreads;

    public Dealer(Env env, Table table, Player[] players) {
        this.env = env;
        this.table = table;
        this.players = players;
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
        playersThreads = new Thread[players.length];
        sets = new ArrayBlockingQueue<>(players.length, true);
        RandomSlots = new ArrayList<>();
        for (int i = 0; i < table.slotToCard.length; i++) {
            RandomSlots.add(i);
        }
        table.players=this.players;
    }

    /**
     * The dealer thread starts here (main loop for the dealer thread).
     */
    @Override
    public void run() {
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
        startPlayersThreads();
        startTime = System.currentTimeMillis();
        while (!shouldFinish()) {
            placeAllCards();
            //table.hints();
            timerLoop();
            removeAllCardsFromTable();
        }
        announceWinners();
        waitForPlayersThread();
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * The inner loop of the dealer thread that runs as long as the countdown did not time out.
     */
    private void timerLoop() {
        while (!terminate && time > 0) {
            updateTimerDisplay(false);
            removeCardsFromTable();
            placeCardsOnTable();
        }
    }

    /**
     * Called when the game should be terminated due to an external event.
     */
    public void terminate() {
        // TODO implement
        synchronized (this){
            notifyAll();
        }
        waitForPlayersThread();
        terminate = true;
    }

    /**
     * Check if the game should be terminated or the game end conditions are met.
     *
     * @return true iff the game should be finished.
     */
    private boolean shouldFinish() {
        return terminate || env.util.findSets(deck, 1).size() == 0;
    }

    /**
     * Checks cards should be removed from the table and removes them.
     */
    private void removeCardsFromTable() {
        // TODO implement
        if (sets.size() > 0) {
            int[] toCheck = sets.remove();
            for (int i = 1; i < toCheck.length; i++) {
                if (!players[toCheck[0]].playerTokens.contains(toCheck[i]) | toCheck.length<4){
                    players[toCheck[0]].isLegal = -1;
                    players[toCheck[0]].playerThread.interrupt();
                    return;
                }
            }
            int[] set = slotsToCards(Arrays.copyOfRange(toCheck, 1, toCheck.length));
            if (env.util.testSet(set)) {
                players[toCheck[0]].isLegal = 0;
                for (int i = 1; i < toCheck.length; i++) {
                    table.removeCard(toCheck[i]);
                }
                updateTimerDisplay(true);
            } else {
                players[toCheck[0]].isLegal = 1;
            }
            players[toCheck[0]].playerThread.interrupt();
        }
    }

    /**
     * Check if any cards can be removed from the deck and placed on the table.
     */
    private void placeCardsOnTable() {
        // TODO implement
        Collections.shuffle(deck);
        Collections.shuffle(RandomSlots);
        for (int i = 0; i < table.slotToCard.length; i++) {
            if (deck.size() > 0) {
                if (table.getCardFromSlot(RandomSlots.get(i)) == null)
                    table.placeCard(deck.remove(0), RandomSlots.get(i));
            }
        }
        for (Player a :
                players) {
            a.canStart=true;
        }
    }

    private void placeAllCards() {
        Collections.shuffle(deck);
        Collections.shuffle(RandomSlots);
        for (int i = 0; i < table.slotToCard.length; i++) {
            if (deck.size() > 0) {
                table.placeCard(deck.remove(0), RandomSlots.get(i));
            }
        }
        updateTimerDisplay(true);
    }


    /**
     * Reset and/or update the countdown and the countdown display.
     */
    private void updateTimerDisplay(boolean reset) {
        // TODO implement
        if (reset) {
            startTime = System.currentTimeMillis();
        }
        long elapsedTime = System.currentTimeMillis() - startTime;
        long timeTillNextDisplayChange = 1000 - (elapsedTime % 1000);
        time = env.config.turnTimeoutMillis - elapsedTime;
        try {
            if (time >= env.config.turnTimeoutWarningMillis)
                Thread.sleep(0);
            else Thread.sleep(0);
        } catch (InterruptedException ignored) {
        }
        if (time >= 0) {
            env.ui.setCountdown(time, time < env.config.turnTimeoutWarningMillis);
        } else env.ui.setCountdown(0, time < env.config.turnTimeoutWarningMillis);
    }

    /**
     * Returns all the cards from the table to the deck.
     */
    private void removeAllCardsFromTable() {
        // TODO implement
        for (Player a :
                players) {
            a.canStart=false;
        }
        Collections.shuffle(RandomSlots);
        for (int i = 0; i < table.slotToCard.length; i++) {
            Integer curr = table.getCardFromSlot(RandomSlots.get(i));
            table.removeCard(RandomSlots.get(i));
            if (curr != null) {
                deck.add(curr);
            }
        }
    }

    /**
     * Check who is/are the winner/s and displays them.
     */
    private void announceWinners() {
        // TODO implement
        int max_point = 0;
        int[] winners;
        int c = 0;
        for (int i = 0; i < players.length; i++) {
            if (players[i].getScore() > max_point)
                max_point = players[i].getScore();
        }
        for (int i = 0; i < players.length; i++) {
            if (players[i].getScore() == max_point) {
                c++;
            }
        }
        winners = new int[c];
        int index = 0;
        for (int i = 0; i < players.length; i++) {
            if (players[i].getScore() == max_point) {
                winners[index] = players[i].id;
                index++;
            }
        }
        env.ui.announceWinner(winners);
    }

    private void startPlayersThreads() {
        for (int i = 0; i < players.length; i++) {
            playersThreads[i] = new Thread(players[i], "Player " + players[i].id);
            playersThreads[i].start();
        }
    }

    private void waitForPlayersThread() {
        for (int i = playersThreads.length-1; i >=0; i--) {
            try {
                players[i].terminate();
                players[i].playerThread.interrupt();
                playersThreads[i].join();
            } catch (InterruptedException ignored) {
            }
        }
    }

    private int[] slotsToCards(int[] a) {
        int[] ans = new int[a.length];
        for (int i = 0; i < ans.length; i++) {
            ans[i] = table.slotToCard[a[i]];
        }
        return ans;
    }

    /**
     * these functions are for testing
     */

    public void placeAllCards_Test(){
        Collections.shuffle(deck);
        Collections.shuffle(RandomSlots);
        for (int i = 0; i < table.slotToCard.length; i++) {
            if (deck.size() > 0) {
                table.placeCard(deck.remove(0), RandomSlots.get(i));
            }
        }
    }
    public void removeAllCardsFromTable_Test(){
        for (Player a :
                players) {
            a.canStart=false;
        }
        Collections.shuffle(RandomSlots);
        for (int i = 0; i < table.slotToCard.length; i++) {
            Integer curr = table.getCardFromSlot(RandomSlots.get(i));
            table.removeCard(RandomSlots.get(i));
            if (curr != null) {
                deck.add(curr);
            }
        }
    }

}
