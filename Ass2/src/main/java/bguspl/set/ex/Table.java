package bguspl.set.ex;

import bguspl.set.Env;

import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

/**
 * This class contains the data that is visible to the player.
 *
 * @inv slotToCard[x] == y iff cardToSlot[y] == x
 */
public class Table {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Mapping between a slot and the card placed in it (null if none).
     */
    protected final Integer[] slotToCard; // card per slot (if any)

    /**
     * Mapping between a card and the slot it is in (null if none).
     */
    protected final Integer[] cardToSlot; // slot per card (if any)

    /**
     * this is a lock for the table - each slot has a lock of its own.
     */
    public Object[] locks;

    public int[][] tokens;

    Player[] players;

    /**
     * Constructor for testing.
     *
     * @param env        - the game environment objects.
     * @param slotToCard - mapping between a slot and the card placed in it (null if
     *                   none).
     * @param cardToSlot - mapping between a card and the slot it is in (null if
     *                   none).
     */
    public Table(Env env, Integer[] slotToCard, Integer[] cardToSlot) {

        this.env = env;
        this.slotToCard = slotToCard; // 12
        this.cardToSlot = cardToSlot; // 81
        locks = new Object[slotToCard.length];
        for (int i = 0; i < locks.length; i++) {
            locks[i] = new Object();
        }
        tokens = new int[slotToCard.length][env.config.players];
        for (int i = 0; i < slotToCard.length; i++) {
            for (int j = 0; j < env.config.players; j++) {
                tokens[i][j] = 0;
            }
        }
    }

    /**
     * Constructor for actual usage.
     *
     * @param env - the game environment objects.
     */
    public Table(Env env) {
        this(env, new Integer[env.config.tableSize], new Integer[env.config.deckSize]);
    }

    /**
     * This method prints all possible legal sets of cards that are currently on the
     * table.
     */
    public void hints() {
        List<Integer> deck = Arrays.stream(slotToCard).filter(Objects::nonNull).collect(Collectors.toList());
        env.util.findSets(deck, Integer.MAX_VALUE).forEach(set -> {
            StringBuilder sb = new StringBuilder().append("Hint: Set found: ");
            List<Integer> slots = Arrays.stream(set).mapToObj(card -> cardToSlot[card]).sorted()
                    .collect(Collectors.toList());
            int[][] features = env.util.cardsToFeatures(set);
            System.out.println(
                    sb.append("slots: ").append(slots).append(" features: ").append(Arrays.deepToString(features)));
        });
    }

    /**
     * Count the number of cards currently on the table.
     *
     * @return - the number of cards on the table.
     */
    public int countCards() {
        int cards = 0;
        for (Integer card : slotToCard)
            if (card != null)
                ++cards;
        return cards;
    }

    /**
     * Places a card on the table in a grid slot.
     *
     * @param card - the card id to place in the slot.
     * @param slot - the slot in which the card should be placed.
     * @post - the card placed is on the table, in the assigned slot.
     */
    public void placeCard(int card, int slot) {
        try {
            Thread.sleep(env.config.tableDelayMillis);
        } catch (InterruptedException ignored) {
        }

        // TODO implement
        // sync the slot when adding
        synchronized (locks[slot]) {
            cardToSlot[card] = slot;
            slotToCard[slot] = card;
            env.ui.placeCard(card, slot);
        }
    }

    /**
     * Removes a card from a grid slot on the table.
     *
     * @param slot - the slot from which to remove the card.
     */
    public void removeCard(int slot) {
        try {
            Thread.sleep(env.config.tableDelayMillis);
        } catch (InterruptedException ignored) {
        }
        synchronized (locks[slot]) {
            if (slotToCard[slot] != null) {
                for (int i = 0; i < tokens[slot].length; i++) {
                    if (tokens[slot][i] == 1) {
                        removeToken(i, slot);
                    }
                }
                cardToSlot[slotToCard[slot]] = null;
                slotToCard[slot] = null;
                env.ui.removeCard(slot);
            }
        }
    }

    /**
     * Places a player token on a grid slot.
     *
     * @param player - the player the token belongs to.
     * @param slot   - the slot on which to place the token.
     */
    public void placeToken(int player, int slot) {
        // TODO implement
        if (slotToCard[slot] != null) {
            synchronized (locks[slot]) {
                tokens[slot][player] = 1;
                players[player].playerTokens.add(slot);
                env.ui.placeToken(player, slot);
            }
        }
    }

    /**
     * Removes a token of a player from a grid slot.
     *
     * @param player - the player the token belongs to.
     * @param slot   - the slot from which to remove the token.
     * @return - true iff a token was successfully removed.
     */
    public boolean removeToken(int player, int slot) {
        // TODO implement
        synchronized (locks[slot]) {
            if (tokens[slot][player] == 1) {
                tokens[slot][player] = 0;
                players[player].playerTokens.remove((Integer) slot);
                env.ui.removeToken(player, slot);
                return true;
            }
            return false;
        }
    }

    public Integer getCardFromSlot(int slot) {
        return slotToCard[slot];
    }

    /**
     *These functions are for Testing
     */
    public boolean ifTokenExists(int playerId, int slot) {
        if (tokens[slot][playerId] == 1)
            return true;
        return false;
    }

    public void PlaceTokensForTesting(int id,int slot){
        if(slotToCard[slot]!=null){
            tokens[slot][id]=1;
            env.ui.placeToken(id, slot);
        }
    }
}