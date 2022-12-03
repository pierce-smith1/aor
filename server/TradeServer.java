import java.util.*;
import java.util.concurrent.*;
import java.net.*;
import java.io.*;
import java.nio.charset.*;
import java.nio.file.*;

import com.sun.net.httpserver.*;

class Item {
    public short code;
    public byte uses;

    public Item() {
        this((short) 0, (byte) 0);
    }

    public Item(short code, byte uses) {
        this.code = code;
        this.uses = uses;
    }

    @Override
    public String toString() {
        return String.format(":%x;%x", code, uses);
    }
}

class Offer {
    public final static int OFFER_SIZE = 3;

    public Item[] items = new Item[OFFER_SIZE];

    public Offer() {
        for (int i = 0; i < OFFER_SIZE; i++) {
            items[i] = new Item();
        }
    }

    @Override
    public String toString() {
        StringBuilder s = new StringBuilder("+");

        for (int i = 0; i < OFFER_SIZE; i++) {
            s.append(items[i]);
        }

        return s.toString();
    }
}

class Offers {
    public Map<Long, Offer> map = Collections.synchronizedMap(new HashMap<>());

    public Offers(Path dataPath) throws IOException {
        if (!Files.exists(dataPath)) {
            Files.createFile(dataPath);
            return;
        }

        String data = new String(Files.readAllBytes(dataPath), "UTF-8");

        String[] offers = data.split("#");
        for (int i = 1; i < offers.length; i++) {
            long gameId = Long.parseLong(offers[i].split("\\+")[0], 16);
            map.put(gameId, new Offer());

            String[] items = offers[i].split(":");

            for (int j = 1; j < items.length; j++) {
                short code = Short.parseShort(items[j].split(";")[0], 16);
                byte uses = Byte.parseByte(items[j].split(";")[1], 16);
                map.get(gameId).items[j - 1] = new Item(code, uses);
            }
        }
    }

    @Override
    public String toString() {
        StringBuilder s = new StringBuilder("-");

        for (Map.Entry<Long, Offer> entry : map.entrySet()) {
            s.append(String.format("#%x%s", entry.getKey(), entry.getValue()));
        }

        return s.toString();
    }
}

public class TradeServer {
    public static Path dataPath = Path.of("tsstate");
    public final static int PORT = 10241;

    public Offers offers = new Offers(dataPath);

    public Map<Long, Long> agreements = Collections.synchronizedMap(new HashMap<>());
    public ScheduledThreadPoolExecutor timer = new ScheduledThreadPoolExecutor(1);
    public Map<Long, String> identities = Collections.synchronizedMap(new HashMap<>());
    public TradePingServer pingServer = new TradePingServer();
    public HttpServer server;

    public TradeServer() throws IOException {
        server = HttpServer.create(new InetSocketAddress(PORT), 0);

        server.createContext("/offeradd/", this::offerAdd);
        server.createContext("/update/", this::update);
        server.createContext("/updateidentities/", this::updateIdentities);
        server.createContext("/accept/", this::accept);
        server.createContext("/unaccept/", this::unaccept);
        server.createContext("/agreements/", this::agreements);
        server.createContext("/identify/", this::identify);

        timer.scheduleWithFixedDelay(() -> {
            pingServer.ping(TradePingServer.MT_IDENTIFY);
        }, 0, 1, TimeUnit.MINUTES);
    }

    public static void main(String[] args) throws IOException {
        TradeServer s = new TradeServer();
        try {
            s.server.start();
            s.pingServer.listen();
        } finally {
            s.pingServer.server.close();
        }
    }

    public void saveServerState() throws IOException {
        Files.write(dataPath, offers.toString().getBytes("UTF-8"));
    }

    public void offerAdd(HttpExchange exchange) throws IOException {
        try {
            String request = new String(exchange.getRequestBody().readAllBytes(), "UTF-8");
            System.out.printf("Request to offeradd: %s\n", request);

            String[] args = request.split(";");

            long gameId = Long.parseLong(args[0], 16);
            int index = Integer.parseInt(args[1], 16);
            short itemCode = Short.parseShort(args[2], 16);
            byte itemUses = Byte.parseByte(args[3], 16);

            if (!offers.map.containsKey(gameId)) {
                offers.map.put(gameId, new Offer());
            }
            offers.map.get(gameId).items[index] = new Item(itemCode, itemUses);

            exchange.sendResponseHeaders(200, 0);
            saveServerState();
            pingServer.ping(TradePingServer.MT_UPDATE);
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }

    public void update(HttpExchange exchange) throws IOException {
        try {
            System.out.printf("Request for update\n");

            String response = offers.toString();
            exchange.sendResponseHeaders(200, response.getBytes("UTF-8").length);
            exchange.getResponseBody().write(response.getBytes("UTF-8"));
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }

    public void updateIdentities(HttpExchange exchange) throws IOException {
        try {
            System.out.printf("Request for updated identities\n");

            StringBuilder response = new StringBuilder();
            for (Map.Entry<Long, String> entry : identities.entrySet()) {
                response.append(":");
                response.append(Long.toString(entry.getKey(), 16));
                response.append(";");
                response.append(entry.getValue());
            }
            exchange.sendResponseHeaders(200, response.toString().getBytes("UTF-8").length);
            exchange.getResponseBody().write(response.toString().getBytes("UTF-8"));
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }

    public void accept(HttpExchange exchange) throws IOException {
        try {
            String request = new String(exchange.getRequestBody().readAllBytes(), "UTF-8");
            System.out.printf("Request to accept: %s\n", request);

            long acceptingGame = Long.parseLong(request.split(";")[0], 16);
            long acceptedGame = Long.parseLong(request.split(";")[1], 16);

            agreements.put(acceptingGame, acceptedGame);

            if (agreements.containsKey(acceptedGame) && agreements.get(acceptedGame) == acceptingGame) {
                System.out.printf("Notifying %d and %d that they should trade!\n", acceptingGame, acceptedGame);
                agreements.remove(acceptingGame);
                agreements.remove(acceptedGame);
                pingServer.ping(TradePingServer.MT_EXECUTE);
            }

            pingServer.ping(TradePingServer.MT_UPDATEAGREEMENTS);
            exchange.sendResponseHeaders(200, 0);
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }

    public void unaccept(HttpExchange exchange) throws IOException {
        try {
            String request = new String(exchange.getRequestBody().readAllBytes(), "UTF-8");
            System.out.printf("Request to unaccept: %s\n", request);

            agreements.remove(Long.parseLong(request, 16));

            exchange.sendResponseHeaders(200, 0);
            pingServer.ping(TradePingServer.MT_UPDATEAGREEMENTS);
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }


    public void agreements(HttpExchange exchange) throws IOException {
        try {
            System.out.printf("Request for agreements\n");

            StringBuilder response = new StringBuilder();
            for (Map.Entry<Long, Long> entry : agreements.entrySet()) {
                response.append(":");
                response.append(Long.toString(entry.getKey(), 16));
                response.append(";");
                response.append(Long.toString(entry.getValue(), 16));
            }
            exchange.sendResponseHeaders(200, response.toString().getBytes("UTF-8").length);
            exchange.getResponseBody().write(response.toString().getBytes("UTF-8"));
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }

    public void identify(HttpExchange exchange) throws IOException {
        try {
            String request = new String(exchange.getRequestBody().readAllBytes(), "UTF-8");
            System.out.printf("Request to identify: %s\n", request);

            long gameId = Long.parseLong(request.split(";")[0], 16);
            String tribeName = request.split(";")[1];

            identities.put(gameId, tribeName);
            exchange.sendResponseHeaders(200, 0);
        } catch (Exception e) {
            e.printStackTrace();
            exchange.sendResponseHeaders(500, 0);
        } finally {
            exchange.close();
        }
    }
}

class TradePingServer {
    public final static byte MT_IGNORE = 0;
    public final static byte MT_UPDATE = 'u';
    public final static byte MT_UPDATEAGREEMENTS = 'a';
    public final static byte MT_EXECUTE = 'e';
    public final static byte MT_IDENTIFY = 'i';

    public final static int PORT = 10242;

    public ServerSocket server = new ServerSocket(PORT);
    public List<Socket> connections = Collections.synchronizedList(new ArrayList<>());
    public byte messageCode = MT_IGNORE;

    public TradePingServer() throws IOException { }

    public void listen() throws IOException {
        while (true) {
            Socket connection = server.accept();
            connections.add(connection);

            System.out.printf("New pings connection from %s\n", connection.getInetAddress());

            Thread holder = new Thread(() -> {
                try {
                    while (true) {
                        synchronized (server) {
                            System.out.println("Pings waiting");
                            server.wait();

                            System.out.printf("Echoing %c\n", messageCode);
                            connection.getOutputStream().write(messageCode);
                            connection.getOutputStream().flush();
                        }
                    }
                } catch (InterruptedException | IOException e) {
                } finally {
                    try {
                        connections.remove(connection);
                        connection.close();
                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }
            });
            holder.start();
        }
    }

    public void ping(byte message) {
        synchronized (server) {
            messageCode = message;
            server.notifyAll();
        }
    }
}
