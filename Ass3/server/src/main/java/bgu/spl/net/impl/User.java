package bgu.spl.net.impl;

import java.util.HashMap;

public class User {
    private String username;
    private String password;
    private boolean online;
    private HashMap<String, String> topics;// the key is the topic name and the value is the id for the user in that topic
                                           

    public User(String username, String pass) {
        this.username = username;
        this.password = pass;
        online = false;
        topics = new HashMap<>();
    }

    public void connect() {
        online = true;
    }

    public void disconnect() {
        online = false;
        topics = new HashMap<>();
    }

    public void addtopic(String topic, String id) {
        topics.put(topic, id);
    }

    public void removetopic(String topic) {
        topics.remove(topic);
    }

    public String getUserName() {
        return username;
    }

    public Boolean isOnline() {
        return online;
    }

    public String getSubscriptionId(String topic) {
        if (topics.keySet().contains(topic)) {
            return topics.get(topic);
        }
        return null;
    }

    public String getPassWord() {
        return password;
    }

    public String getTopic(String subid) {
        for (String key : topics.keySet()) {
            if (subid.equals(topics.get(key))) {
                return key;
            }
        }
        return null;
    }
}
