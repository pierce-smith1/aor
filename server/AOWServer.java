import java.net.InetSocketAddress;
import java.io.*;
import java.util.*;

import com.sun.net.httpserver.*;

public class AOWServer {
    public static int AOW_PORT = 10241;
    public static String DATA_FILENAME = "aows.bin";

    private HttpServer server;
    private File dataFile;

    public class Item {
        short code;
        long id;
        byte uses_left;
        short intent;

        public Item(DataInput in) {
            try {
                code = in.readShort();
                id = in.readLong();
                uses_left = in.readByte();
                intent = in.readShort();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        public void serialize(DataOutput out) {
            try {
                out.writeShort(code);
                out.writeLong(id);
                out.writeByte(uses_left);
                out.writeShort(intent);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        @Override
        public int hashCode() {
            return Objects.hash(code, id, uses_left, intent);
        }
    }

    public class OfferHistory {
        public HashMap<Item, List<Item>> history;

        public OfferHistory(File dataFile) throws IOException {
            this(new DataInputStream(new FileInputStream(dataFile)));
        }

        public OfferHistory(DataInput in) {
            try {
                history = new HashMap<>();

                int numEntries = in.readInt();
                for (int i = 0; i < numEntries; i++) {
                    Item key = new Item(in);
                    List<Item> items = new ArrayList<>();
                    int numItems = in.readInt();
                    for (int j = 0; j < numItems; j++) {
                        items.add(new Item(in));
                    }

                    history.put(key, items);
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        public void serialize(File dataFile) {

        }
    }

    public AOWServer() {
        try {
            dataFile = new File(DATA_FILENAME);

            server = HttpServer.create(new InetSocketAddress(AOW_PORT), 0);
            server.createContext("offer", this::handleOffer);

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public void handleOffer(HttpExchange exchange) {
        try {
            OfferHistory offers = new OfferHistory(dataFile);
            DataInputStream input = new DataInputStream(exchange.getRequestBody());

            Item keyItem = new Item(input);
            if (offers.history.containsKey(keyItem)) {
                List<Item> itemsToReturn = offers.history.get(keyItem);

                ByteArrayOutputStream bytesOut = new ByteArrayOutputStream();
                DataOutputStream out = new DataOutputStream(bytesOut);
                out.writeInt(itemsToReturn.size());
                for (Item item : itemsToReturn) {
                    item.serialize(out);
                }

                exchange.sendResponseHeaders(200, bytesOut.toByteArray().length);
                exchange.getResponseBody().write(bytesOut.toByteArray());

                offers.history.remove(keyItem);
            } else {

                exchange.sendResponseHeaders(200, 0);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }
}
