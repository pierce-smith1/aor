import java.util.*;
import java.util.concurrent.*;
import java.net.*;
import java.io.*;
import java.nio.charset.*;
import java.nio.file.*;

import com.sun.net.httpserver.*;

class TradeWorker extends Thread {
    public TradeServer server;
    public Socket connection;
    public DataInputStream in;
    public DataOutputStream out;
    public long gameId;

    public TradeWorker(TradeServer server, Socket connection, long gameId) throws IOException {
        this.server = server;
        this.connection = connection;
        this.gameId = gameId;

        in = new DataInputStream(connection.getInputStream());
        out = new DataOutputStream(connection.getOutputStream());
    }

    @Override
    public void run() {
        try {
            while (true) {
                synchronized (in) {
                    byte messageCode = in.readByte();
                    dispatch(messageCode);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
                System.out.printf("Lost connection from %s, gameId %x\n", connection.getInetAddress(), gameId);

                DataOutputStream others = server.sendAllOthers(gameId);

                others.writeByte(TradeServer.MT_TRIBEAVAILABILITYCHANGED);
                others.writeLong(gameId);
                others.writeBoolean(false);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    public void dispatch(byte messageCode) throws IOException {
        switch (messageCode) {
            case TradeServer.MT_OFFERCHANGED: {
                System.out.printf("[%x] notifying offer change\n", gameId);
                offerChanged();
                break;
            }
            case TradeServer.MT_AGREEMENTCHANGED: {
                System.out.printf("[%x] notifying agreement change\n", gameId);
                agreementChanged();
                break;
            }
            case TradeServer.MT_TRIBEAVAILABILITYCHANGED: {
                System.out.printf("[%x] notifying availability change\n", gameId);
                tribeAvailabilityChanged();
                break;
            }
            case TradeServer.MT_WANTGAMESTATE: {
                System.out.printf("[%x] wants game state\n", gameId);
                wantGameState();
                break;
            }
            default: {
                System.out.printf("[%x] ! Got unknown messageCode %x (%c)\n", gameId, messageCode, messageCode);
            }
        }
    }

    public void offerChanged() throws IOException {
        short itemCode = in.readShort();
        byte  itemUses = in.readByte();
        short index    = in.readShort();

        DataOutputStream others = server.sendAllOthers(gameId);

        others.writeByte(TradeServer.MT_OFFERCHANGED);
        others.writeLong(gameId);
        others.writeShort(itemCode);
        others.writeByte(itemUses);
        others.writeShort(index);
    }

    public void agreementChanged() throws IOException {
        long partnerId = in.readLong();
        boolean accepted  = in.readBoolean();

        DataOutputStream partner = server.send(partnerId);

        synchronized (partner) {
            partner.writeByte(TradeServer.MT_AGREEMENTCHANGED);
            partner.writeLong(gameId);
            partner.writeBoolean(accepted);
        }
    }

    public void tribeAvailabilityChanged() throws IOException {
        boolean nowAvailable = in.readBoolean();

        DataOutputStream others = server.sendAllOthers(gameId);

        others.writeByte(TradeServer.MT_TRIBEAVAILABILITYCHANGED);
        others.writeLong(gameId);
        others.writeBoolean(nowAvailable);
    }

    public void wantGameState() throws IOException {
        DataOutputStream others = server.sendAllOthers(gameId);

        others.writeByte(TradeServer.MT_WANTGAMESTATE);
        others.writeLong(gameId); // please address your game state to this id
    }

    public void myInfo() throws IOException {
        long reportTo = in.readLong();
        short itemCode1 = in.readShort();
        byte itemUses1 = in.readByte();
        short itemCode2 = in.readShort();
        byte itemUses2 = in.readByte();
        short itemCode3 = in.readShort();
        byte itemUses3 = in.readByte();
        boolean acceptingTrade = in.readBoolean();

        DataOutputStream client = server.send(reportTo);

        synchronized (client) {
            client.writeByte(TradeServer.MT_MYINFO);
            client.writeLong(gameId);
            client.writeShort(itemCode1);
            client.writeByte(itemUses1);
            client.writeShort(itemCode2);
            client.writeByte(itemCode2);
            client.writeShort(itemCode3);
            client.writeByte(itemCode3);
            client.writeBoolean(acceptingTrade);
        }
    }
}

public class TradeServer {
    public final static int PORT = 10241;

    public ServerSocket server = new ServerSocket(PORT);
    public Map<Long, TradeWorker> workers = Collections.synchronizedMap(new HashMap<>());

    public final static byte MT_IGNORE = 0;
    public final static byte MT_OFFERCHANGED = 'o';
    public final static byte MT_AGREEMENTCHANGED = 'g';
    public final static byte MT_TRIBEAVAILABILITYCHANGED = 't';
    public final static byte MT_WANTGAMESTATE = 'w';
    public final static byte MT_MYINFO = 'i';

    public TradeServer() throws IOException { }

    public static void main(String[] args) throws IOException {
        TradeServer server = new TradeServer();
        server.listen();
    }

    public void listen() throws IOException {
        while (true) {
            Socket connection = server.accept();

            DataInputStream in = new DataInputStream(connection.getInputStream());
            Long gameId = in.readLong();

            System.out.printf("New connection from %s, gameId %x\n", connection.getInetAddress(), gameId);

            TradeWorker worker = new TradeWorker(this, connection, gameId);
            workers.put(gameId, worker);
            worker.start();
        }
    }

    public DataOutputStream send(long gameId) {
        return workers.get(gameId).out;
    }

    public DataOutputStream sendAllOthers(long gameId) {
        return new DataOutputStream(new OutputStream() {
            public void write(int b) throws IOException {
                for (Map.Entry<Long, TradeWorker> entry : workers.entrySet()) {
                    if (entry.getKey() != gameId) {
                        DataOutputStream out = entry.getValue().out;
                        synchronized (out) { // LOXXE CITY
                            out.write(b);
                        }
                    }
                }
            }
        });
    }
}
