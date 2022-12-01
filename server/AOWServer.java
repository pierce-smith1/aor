import java.net.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import java.net.http.*;

public class AOWServer {
    public final static int AOW_PORT = 10241;
    public final static String DATA_FILENAME = "aows.bin";

    public final static byte API_VERSION = 1;

    public final static byte REQ_CHECKIN = 0;
    public final static byte REQ_OFFER = 1;

    public final static byte RES_NOTHING_TO_DO = 0;
    public final static byte RES_OFFER = 1;
    public final static byte RES_ERROR = 2;

    private ServerSocket server;
    private File dataFile = new File(DATA_FILENAME);
    private HttpClient client = HttpClient.newHttpClient();
    private OfferHistory history = new OfferHistory();
    private OfferQueue queue = new OfferQueue();

    public static void main(String[] args) {
        try {
            AOWServer server = new AOWServer();
            server.listen();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static abstract class ByteArraySerializable {
        public abstract void serialize(DataOutput out) throws IOException;

        public byte[] asBytes() throws IOException {
            ByteArrayOutputStream bytesOut = new ByteArrayOutputStream();
            DataOutputStream out = new DataOutputStream(bytesOut);

            serialize(out);

            return bytesOut.toByteArray();
        }
    }

    public static class Item extends ByteArraySerializable {
        short code;
        byte uses_left;

        public Item(DataInput in) throws IOException {
            code = in.readShort();
            uses_left = in.readByte();
        }

        @Override
        public void serialize(DataOutput out) throws IOException {
            out.writeShort(code);
            out.writeByte(uses_left);
        }

        @Override
        public int hashCode() {
            return Objects.hash(code, uses_left);
        }

        @Override
        public String toString() {
            return String.format("Item(code %d, uses %d)", code, uses_left);
        }
    }

    public static class Items extends ByteArraySerializable {
        public List<Item> items;
        public long sourceGameId;

        public Items(List<Item> items, long sourceGameId) {
            this.items = items;
            this.sourceGameId = sourceGameId;
        }

        public Items(DataInput in) throws IOException {
            items = new ArrayList<>();

            sourceGameId = in.readLong();
            short size = in.readShort();
            for (int i = 0; i < size; i++) {
                items.add(new Item(in));
            }
        }

        @Override
        public void serialize(DataOutput out) throws IOException {
            out.writeLong(sourceGameId);
            out.writeShort(items.size());
            for (Item item : items) {
                item.serialize(out);
            }
        }

        @Override
        public String toString() {
            return String.format("Items(items %s, sourceGameId %s)", items, sourceGameId);
        }
    }

    public static class OfferRequest extends ByteArraySerializable {
        public short key;
        public Items items;

        public OfferRequest(DataInput in) throws IOException {
            key = in.readShort();
            items = new Items(in);
        }

        public OfferRequest(short key, Items items) {
            this.key = key;
            this.items = items;
        }

        @Override
        public void serialize(DataOutput out) throws IOException {
            out.writeShort(key);
            items.serialize(out);
        }

        @Override
        public String toString() {
            return String.format("Offering(key %d, items %s)", key, items);
        }
    }

    public static class OfferHistory extends ByteArraySerializable {
        public Map<Short, OfferRequest> map;

        public OfferHistory() throws IOException {
            this(new DataInputStream(new ByteArrayInputStream(new byte[] { 0, 0 })));
        }

        public OfferHistory(File dataFile) throws IOException {
            this(new DataInputStream(new FileInputStream(dataFile)));
        }

        public OfferHistory(DataInput in) throws IOException {
            map = Collections.synchronizedMap(new HashMap<Short, OfferRequest>());

            short numEntries = in.readShort();
            for (int i = 0; i < numEntries; i++) {
                short key = in.readShort();
                map.put(key, new OfferRequest(in));
            }
        }

        @Override
        public void serialize(DataOutput out) throws IOException {
            out.writeShort((short) map.size());
            for (Map.Entry<Short, OfferRequest> entry : map.entrySet()) {
                out.writeShort(entry.getKey());
                entry.getValue().serialize(out);
            }
        }
    }

    public static class OfferQueue extends ByteArraySerializable {
        public Map<Long, Items> map;

        public OfferQueue() throws IOException {
            this(new DataInputStream(new ByteArrayInputStream(new byte[] { 0, 0 })));
        }

        public OfferQueue(DataInput in) throws IOException {
            map = Collections.synchronizedMap(new HashMap<Long, Items>());

            short size = in.readShort();
            for (int i = 0; i < size; i++) {
                map.put(in.readLong(), new Items(in));
            }
        }

        @Override
        public void serialize(DataOutput out) throws IOException {
            out.writeShort((short) map.size());
            for (Map.Entry<Long, Items> entry : map.entrySet()) {
                out.writeLong(entry.getKey());
                entry.getValue().serialize(out);
            }
        }
    }

    public AOWServer() throws IOException {
        if (dataFile.exists()) {
            deserialize(new DataInputStream(new FileInputStream(dataFile)));
        } else {
            dataFile.createNewFile();
            serialize(new DataOutputStream(new FileOutputStream(dataFile)));
        }

        server = new ServerSocket(AOW_PORT);
    }

    public void listen() throws IOException {
        while (true) {
            try (Socket connection = server.accept()) {
                DataInputStream input = new DataInputStream(connection.getInputStream());
                DataOutputStream out = new DataOutputStream(connection.getOutputStream());

                byte apiVersion = input.readByte();
                if (apiVersion != API_VERSION) {
                    System.out.println(String.format("Got bad version from %s", connection.getInetAddress()));
                    return;
                }

                long gameId = input.readLong();

                byte messageType = input.readByte();
                switch (messageType) {
                    case REQ_CHECKIN: {
                        handleCheckin(connection, gameId);
                        break;
                    }
                    case REQ_OFFER: {
                        handleOffer(connection, gameId);
                        break;
                    }
                    default: {
                        System.out.println(String.format(
                            "Got unknown message type %d from %s (gameid %d)",
                            messageType,
                            connection.getInetAddress(),
                            gameId)
                        );
                    }
                }
            }
        }
    }

    public void handleCheckin(Socket exchange, long gameId) throws IOException {
        DataInputStream input = new DataInputStream(exchange.getInputStream());
        DataOutputStream out = new DataOutputStream(exchange.getOutputStream());

        if (queue.map.containsKey(gameId)) {
            out.writeByte(RES_OFFER);

            OfferRequest response = new OfferRequest((short) 0, queue.map.get(gameId));
            out.write(response.asBytes());

            queue.map.remove(gameId);
        } else {
            out.writeByte(RES_NOTHING_TO_DO);
        }
    }

    public void handleOffer(Socket exchange, long gameId) throws IOException {
        try {
            DataInputStream input = new DataInputStream(exchange.getInputStream());
            DataOutputStream out = new DataOutputStream(exchange.getOutputStream());

            System.out.println(String.format("Reading offer from %s",
                exchange.getInetAddress().getHostName()
            ));

            OfferRequest thisOffering = new OfferRequest(input);
            System.out.println(String.format("Offering: %s", thisOffering));

            if (history.map.containsKey(thisOffering.key)) {
                System.out.println("Offer key exists");

                OfferRequest originalOffering = history.map.get(thisOffering.key);
                System.out.println(String.format("Original offering from history: %s", originalOffering));

                if (gameId == originalOffering.items.sourceGameId) {
                    out.writeByte(RES_NOTHING_TO_DO);
                    history.map.put(thisOffering.key, thisOffering);
                    return;
                }

                // Send the previously offered items to the game that just offered
                OfferRequest response = new OfferRequest(thisOffering.key, originalOffering.items);
                out.writeByte(RES_OFFER);
                out.write(response.asBytes());
                System.out.println(String.format("Writing response: %s", bytesToString(response.asBytes())));
                history.map.remove(thisOffering.key);

                // Send the items just offered to the previous offerer (by queuing them for the next checkin)
                queue.map.put(originalOffering.items.sourceGameId, thisOffering.items);
            } else {
                System.out.println("Offer key did not exist");
                history.map.put(thisOffering.key, thisOffering);
                out.writeByte(RES_NOTHING_TO_DO);
            }
        } catch (IOException e) {
            e.printStackTrace();
            exchange.close();
        } finally {
            try {
                history.serialize(new DataOutputStream(new FileOutputStream(dataFile)));
            } catch (FileNotFoundException e) {
                exchange.close();
                throw new RuntimeException(e);
            }
        }
    }

    public void serialize(DataOutput out) throws IOException {
        history.serialize(out);
        queue.serialize(out);
    }

    public void deserialize(DataInput in) throws IOException {
        history = new OfferHistory(in);
        queue = new OfferQueue(in);
    }

    public static String bytesToString(byte[] bytes) {
        String string = "";
        for (byte b : bytes) {
            string += String.format("%02x", b);
        }
        return string;
    }
}
