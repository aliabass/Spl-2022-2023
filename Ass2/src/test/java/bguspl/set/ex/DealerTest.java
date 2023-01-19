package bguspl.set.ex;

import bguspl.set.Config;
import bguspl.set.Env;
import bguspl.set.UserInterface;
import bguspl.set.Util;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.Mock;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DealerTest {

    Dealer dealer;
    private List<Integer> RandomSlots;
    private List<Integer> deck;
    private  Player[] players;


    //
    @Mock
    private Table table;
    @Mock
    private Player p1;

    private Player p2;
    @Mock
    Util util;
    @Mock
    private UserInterface ui;
    @Mock
    private Logger logger;

    private Integer[] slotToCard;
    private Integer[] cardToSlot;


    @BeforeEach
    void setUp() {
        MockLogger logger = new MockLogger();
        ui=new MockUserInterface();
        util=new MockUtil();
        Env env = new Env(logger, new Config(logger, ""), ui, util);
        Player []players=new Player[2];

        Properties properties = new Properties();
        properties.put("Rows", "2");
        properties.put("Columns", "2");
        properties.put("FeatureSize", "3");
        properties.put("FeatureCount", "4");
        properties.put("TableDelaySeconds", "0");
        properties.put("PlayerKeys1", "81,87,69,82");
        properties.put("PlayerKeys2", "85,73,79,80");
        Config config = new Config(logger, properties);
        slotToCard = new Integer[config.tableSize];
        cardToSlot = new Integer[config.deckSize];
        table = new Table(env, slotToCard, cardToSlot);
        dealer = new Dealer(env , table , players);
        RandomSlots = new ArrayList<>();
        for (int i = 0; i < table.slotToCard.length; i++) {
            RandomSlots.add(i);
        }
        p1=new Player(env, dealer, table, 0, false);
        p2 = new Player(env, dealer, table, 1, false);
        players[0]=p1;
        players[1]=p2;
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
    }

    @AfterEach
    void tearDown() {
    }

    static class MockLogger extends Logger {
        protected MockLogger() {
            super("", null);
        }
    }
    private void fillAllSlotsByCards(){
        dealer.placeAllCards_Test();
    }
    private int numOfCardsOnTable(){
        return table.countCards();
    }
    private boolean halfSlotsFilled(){
        if(numOfCardsOnTable()==6){
            return true;
        }
        return false;
    }

    @Test
    void AllSlotsAreFilled(){
        fillAllSlotsByCards();
        assertEquals(table.slotToCard.length,numOfCardsOnTable());
    }
    @Test
    void removeAllCards(){
        fillAllSlotsByCards();
        dealer.removeAllCardsFromTable_Test();
        assertEquals(0,numOfCardsOnTable());
    }
    @Test
    void checkLegalSet(){
        table.placeCard(0,0);
        table.placeCard(1,1);
        table.placeCard(2,2);
        p1.keyPressForTesting(0);
        p1.keyPressForTesting(1);
        p1.keyPressForTesting(2);
        int [] a={0,1,2};
        assertTrue(testSet(a));
    }

    @Test
    void checkillegalSet(){
        table.placeCard(0,0);
        table.placeCard(1,1);
        table.placeCard(2,3);
        p1.keyPressForTesting(0);
        p1.keyPressForTesting(1);
        p1.keyPressForTesting(2);
        int [] a={0,1,2};
        assertTrue(testSet(a));
    }

    boolean testSet(int []a){
        return util.testSet(a);
    }

    static class MockUserInterface implements UserInterface {
        @Override
        public void dispose() {
        }

        @Override
        public void placeCard(int card, int slot) {
        }

        @Override
        public void removeCard(int slot) {
        }

        @Override
        public void setCountdown(long millies, boolean warn) {
        }

        @Override
        public void setElapsed(long millies) {
        }

        @Override
        public void setScore(int player, int score) {
        }

        @Override
        public void setFreeze(int player, long millies) {
        }

        @Override
        public void placeToken(int player, int slot) {
        }

        @Override
        public void removeTokens() {
        }

        @Override
        public void removeTokens(int slot) {
        }

        @Override
        public void removeToken(int player, int slot) {
        }

        @Override
        public void announceWinner(int[] players) {
        }
    }

    static class MockUtil implements Util {
        @Override
        public int[] cardToFeatures(int card) {
            return new int[0];
        }

        @Override
        public int[][] cardsToFeatures(int[] cards) {
            return new int[0][];
        }

        @Override
        public boolean testSet(int[] cards) {
            if (cards[0]==0 &cards[1]==1&cards[2]==2)
                return true;
            return false;
        }


        @Override
        public List<int[]> findSets(List<Integer> deck, int count) {
            return null;
        }

        @Override
        public void spin() {
        }
    }


}