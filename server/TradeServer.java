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
    public String tribeName;

    public TradeWorker(TradeServer server, Socket connection, long gameId, String tribeName) throws IOException {
        this.server = server;
        this.connection = connection;
        this.gameId = gameId;
        this.tribeName = tribeName;

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
                server.workers.remove(gameId);
                System.out.printf("Lost connection from %s, gameId %x\n", connection.getInetAddress(), gameId);

                DataOutputStream others = server.sendAllOthers(gameId);

                others.writeByte(TradeServer.MT_TRIBEAVAILABILITYCHANGED);
                others.writeLong(gameId);
                TradeServer.writeString(others, tribeName);
                others.writeBoolean(false);

                others.flush();
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
            case TradeServer.MT_MYINFO: {
                System.out.printf("[%x] got game info\n", gameId);
                myInfo();
                break;
            }
            case TradeServer.MT_EXECUTETRADE: {
                System.out.printf("[%x] executing trade\n", gameId);
                executeTrade();
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

        others.flush();
    }

    public void agreementChanged() throws IOException {
        long partnerId    = in.readLong();
        boolean accepted  = in.readBoolean();

        DataOutputStream partner = server.send(partnerId);

        synchronized (partner) {
            partner.writeByte(TradeServer.MT_AGREEMENTCHANGED);
            partner.writeLong(gameId);
            partner.writeBoolean(accepted);

            partner.flush();
        }
    }

    public void tribeAvailabilityChanged() throws IOException {
        boolean nowAvailable = in.readBoolean();

        DataOutputStream others = server.sendAllOthers(gameId);

        others.writeByte(TradeServer.MT_TRIBEAVAILABILITYCHANGED);
        others.writeLong(gameId);
        TradeServer.writeString(others, tribeName);
        others.writeBoolean(nowAvailable);

        others.flush();
    }

    public void wantGameState() throws IOException {
        System.out.printf("[%x] asking others to notify %s about their game state...\n", gameId, tribeName);
        DataOutputStream others = server.sendAllOthers(gameId);

        others.writeByte(TradeServer.MT_WANTGAMESTATE);
        others.writeLong(gameId);

        others.flush();
    }

    public void myInfo() throws IOException {
        long reportTo          = in.readLong();
        String tribeName       = TradeServer.readString(in);
        short itemCode1        = in.readShort();
        byte itemUses1         = in.readByte();
        short itemCode2        = in.readShort();
        byte itemUses2         = in.readByte();
        short itemCode3        = in.readShort();
        byte itemUses3         = in.readByte();
        short itemCode4        = in.readShort();
        byte itemUses4         = in.readByte();
        short itemCode5        = in.readShort();
        byte itemUses5         = in.readByte();
        boolean acceptingTrade = in.readBoolean();

        DataOutputStream client = server.send(reportTo);

        synchronized (client) {
            client.writeByte(TradeServer.MT_MYINFO);
            client.writeLong(gameId);
            TradeServer.writeString(client, tribeName);
            client.writeShort(itemCode1);
            client.writeByte(itemUses1);
            client.writeShort(itemCode2);
            client.writeByte(itemUses2);
            client.writeShort(itemCode3);
            client.writeByte(itemUses3);
            client.writeShort(itemCode4);
            client.writeByte(itemUses4);
            client.writeShort(itemCode5);
            client.writeByte(itemUses5);
            client.writeBoolean(acceptingTrade);

            client.flush();
        }
    }

    public void executeTrade() throws IOException {
        long tribeId = in.readLong();

        DataOutputStream client = server.send(tribeId);

        synchronized (client) {
            client.writeByte(TradeServer.MT_EXECUTETRADE);

            client.flush();
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
    public final static byte MT_EXECUTETRADE = 'e';

    public TradeServer() throws IOException { }

    public static void main(String[] args) throws IOException {
        TradeServer server = new TradeServer();
        server.listen();
    }

    public void listen() throws IOException {
        while (true) {
            try {
                Socket connection = server.accept();

                DataInputStream in = new DataInputStream(connection.getInputStream());
                Long gameId = in.readLong();
                String tribeName = readString(in);

                System.out.printf("New connection from %s, gameId %x (%s)\n", connection.getInetAddress(), gameId, tribeName);

                TradeWorker worker = new TradeWorker(this, connection, gameId, tribeName);
                workers.put(gameId, worker);
                worker.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public DataOutputStream send(long gameId) {
        System.out.printf("Sending message to %x\n", gameId);
        return workers.get(gameId).out;
    }

    public DataOutputStream sendAllOthers(long gameId) {
        return new DataOutputStream(new OutputStream() {
            public void flush() throws IOException {
                for (Map.Entry<Long, TradeWorker> entry : workers.entrySet()) {
                    if (entry.getKey() != gameId) {
                        DataOutputStream out = entry.getValue().out;
                        synchronized (out) {
                            out.flush();
                        }
                    }
                }
            }

            public void write(int b) throws IOException {
                for (Map.Entry<Long, TradeWorker> entry : workers.entrySet()) {
                    if (entry.getKey() != gameId) {
                        DataOutputStream out = entry.getValue().out;
                        synchronized (out) {
                            out.write(b);
                        }
                    }
                }
            }
        });
    }

    public static void writeString(DataOutputStream out, String s) throws IOException {
        out.writeByte((byte) s.length());
        out.writeBytes(s);
    }

    public static String readString(DataInputStream in) throws IOException {
        byte size = in.readByte();
        byte[] bytes = new byte[size];
        in.read(bytes);
        return new String(bytes, "UTF-8");
    }
}
