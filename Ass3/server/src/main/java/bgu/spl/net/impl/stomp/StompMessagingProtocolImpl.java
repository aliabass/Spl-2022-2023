package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;

import java.lang.String;
import java.util.HashMap;
import java.util.Map;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    private int id;
    private ConnectionsImpl<String> conn;
    private boolean shouldTerminate;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.id = connectionId;
        this.conn = (ConnectionsImpl<String>) connections;
        shouldTerminate = false;
    }

    @Override
    public void process(String message) {
        String command = getComand(message);
        Map<String, String> headers = new HashMap<>();
        getHeaders(headers, message);
        String body = getBody(message);
        switch (command) {
            case "CONNECT": {
                String frame = connectCommand(headers);
                conn.send(id, frame);
                break;
            }
            case "SEND": {
                sendCommand(headers, body);
                break;
            }
            case "SUBSCRIBE": {
                String frame = subCommand(headers);
                conn.send(id, frame);
                break;
            }
            case "UNSUBSCRIBE": {
                String frame = unsubCommand(headers);
                conn.send(id, frame);
                break;
            }
            case "DISCONNECT": {
                String frame = disconnectCommand(headers);
                conn.send(id, frame);
                conn.disconnect(id);
                break;
            }
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    public String getComand(String s) {
        String[] a = s.split("\n");
        return a[0];
    }

    public void getHeaders(Map<String, String> h, String s) {
        String[] lines = s.split("\n");
        for (int i = 1; i < lines.length; i++) {
            // Headers are in the format "key:value"
            String[] parts = lines[i].split(":");
            if (parts.length<2)
                break;
            h.put(parts[0], parts[1]);
        }
    }

    public String getBody(String s) {
        int begIndex = s.indexOf("\n\n") + 1;
        return s.substring(begIndex);
    }

    public String connectCommand(Map<String, String> headers) {
        String username = headers.get("login");
        String password = headers.get("passcode");
        String version = headers.get("accept-version");
        String ans = "";
        String error = "";
        if (conn.userExsist(username)) {
            if (conn.checkPass(username, password)) {
                if (!conn.isUserConnected(username)) {
                    ans = "CONNECTED\n" + "version:" + version + "\n\n";
                    conn.connectUser(username,this.id);
                    return ans;
                } else {
                    error = "ERROR\n" + "message:User is already connected\n\n";
                    return error;
                }
            } else {
                error = "ERROR\n" + "message:Wrong password\n\n";
                return error;
            }
        } else {
            conn.addUser(username, password, id);
            ans = "CONNECTED\n" + "version:" + version + "\n\n";
            return ans;
        }
    }

    public void sendCommand(Map<String, String> headers, String body) {
        String des=headers.get("destination");
        HashMap<Integer,String> toSend=conn.getSubsfromtopic(des);
        for (Integer x : toSend.keySet()) {
            String ans="MESSAGE\n"+"subscription:"+toSend.get(x)+"\n"+"message - id :"+conn.getMsgId()+"\n"+"destination :"+des+"\n\n"+body;
            conn.send(x, ans);
        }
    }

    public String subCommand(Map<String, String> headers) {
        String des=headers.get("destination");
        String id=headers.get("id");
        String receiptId=headers.get("receipt");
        String ans="";
        conn.addUserToTopic(des,id , this.id);
        ans="RECEIPT\n"+"receipt-id:"+receiptId+"\n\n";
        return ans;
    }

    public String unsubCommand(Map<String, String> headers) {
        String subid=headers.get("id");
        String receiptId=headers.get("receipt");
        String ans="";
        String topic=conn.getTopic(this.id,subid);
        if(topic!=null){
            conn.removeUserFromTopic(topic, subid, this.id);
            ans="RECEIPT\n"+"receipt-id:"+receiptId+"\n\n";
            return ans;
        }
        ans="ERROR\n" + "message:user is not subscribed to this topic\n\n";
        return ans;
    }

    public String disconnectCommand(Map<String, String> headers) {
        String receiptId=headers.get("receipt");
        shouldTerminate=true;
        String ans="RECEIPT\n"+"receipt-id:"+receiptId+"\n\n";
        return ans;
    }
}
