package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.impl.User;

public class ConnectionsImpl<T> implements Connections<T> {
    private static ConnectionsImpl<String> instance = null;
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> handlers;
    private ConcurrentHashMap<String, User> userNames;
    private ConcurrentHashMap<Integer, User> Users;
    private ConcurrentHashMap<String, LinkedList<ConnectionHandler<T>>> allTopics;
    private AtomicInteger id;
    private AtomicInteger msgId;

    private ConnectionsImpl() {
        handlers = new ConcurrentHashMap<>();
        userNames = new ConcurrentHashMap<>();
        Users = new ConcurrentHashMap<>();
        allTopics = new ConcurrentHashMap<>();
        id = new AtomicInteger(0);
        msgId=new AtomicInteger(0);
    }

    public static synchronized ConnectionsImpl<String> getInstance() {
        if (instance == null) {
            instance = new ConnectionsImpl<String>();
        }
        return instance;
    }

    @Override
    public boolean send(int connectionId, Object msg) {
        if (handlers.get(connectionId) == null) {
            return false;
        }
        handlers.get(connectionId).send((T)msg);
        return true;
    }

    @Override
    public void send(String channel, Object msg) {
        LinkedList<ConnectionHandler<T>> toSend=allTopics.get(channel);
        for (ConnectionHandler<T> a: toSend) {
            a.send((T)msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        Users.get(connectionId).disconnect();
        ConnectionHandler<T> toRemove=handlers.get(connectionId);
        for (String t: allTopics.keySet()) {
            if(allTopics.get(t).contains(toRemove)){
                allTopics.get(t).remove(toRemove);
                if(allTopics.get(t).size()==0){
                    allTopics.remove(t);
                }
            }
        }
        Users.remove(connectionId);
        handlers.remove(connectionId);

    }

    public int getId() {
        return id.getAndIncrement();
    }

    public int getMsgId(){
        return msgId.getAndIncrement();
    }

    public void addHandler(int id, ConnectionHandler<T> h) {
        handlers.put(id, h);
    }

    public void addUser(String user, String pass, int id) {
        User a = new User(user, pass);
        a.connect();
        userNames.put(user, a);
        Users.put(id, a);
    }

    public boolean userExsist(String username) {
        return userNames.keySet().contains(username);
    }

    public String getSubscriptionId(String topic,int id){
        return Users.get(id).getSubscriptionId(topic);
    }

    public boolean checkPass(String user, String pass) {
        if (userNames.get(user).getPassWord().equals(pass)) {
            return true;
        }
        return false;
    }

    public boolean isUserConnected(String user) {
        return userNames.get(user).isOnline();
    }

    public void addUserToTopic(String topic,String subId,int id){
        if(allTopics.keySet().contains(topic)){
            ConnectionHandler<T> toAdd=handlers.get(id);
            allTopics.get(topic).add(toAdd);
            Users.get(id).addtopic(topic, subId);
        }else{
            allTopics.put(topic, new LinkedList<ConnectionHandler<T>>());
            ConnectionHandler<T> toAdd=handlers.get(id);
            allTopics.get(topic).add(toAdd);
            Users.get(id).addtopic(topic, subId);
        }
    }

    public String getTopic(int id,String subid){
        return Users.get(id).getTopic(subid);
    }

    public void removeUserFromTopic(String topic,String subId,int id){
        (allTopics.get(topic)).remove(handlers.get(id));
        Users.get(id).removetopic(topic);
        if(allTopics.get(topic).size()==0){
            allTopics.remove(topic);
        }
    }

    public HashMap<Integer,String> getSubsfromtopic(String topic){
        HashMap<Integer,String> ans=new HashMap<>(); 
        for (Integer x : Users.keySet()) {
            if(Users.get(x).getSubscriptionId(topic)!=null){
                ans.put(x, Users.get(x).getSubscriptionId(topic));
            }
        }
        return ans;
    }

    public void connectUser(String user,int id){
        userNames.get(user).connect();
        Users.put(id, userNames.get(user));
    }
}
