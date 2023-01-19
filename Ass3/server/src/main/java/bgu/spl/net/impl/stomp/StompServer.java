package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        int port = Integer.parseInt(args[0]);
        if (args[1].equals("tpc")) {
            Server.threadPerClient(
                    port,
                    () -> new StompMessagingProtocolImpl(),
                    LineMessageEncoderDecoder::new).serve();
        }
        if (args[1].equals("reactor")) {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port,
                    () -> new StompMessagingProtocolImpl(),
                    LineMessageEncoderDecoder::new).serve();
        }
    }
}
