import java.util.*;
import java.util.concurrent.*;

/**
 * EventMetadata class - Contains domain-specific metadata for events
 */
class EventMetadata {
    private String domainId; // orderId : "order-456"
    private String ownerId; // userId : "user-123"

    public EventMetadata(String domainId, String ownerId) {
        this.domainId = domainId;
        this.ownerId = ownerId;
    }

    public String getDomainId() {
        return domainId;
    }

    public String getOwnerId() {
        return ownerId;
    }

    @Override
    public String toString() {
        return "EventMetadata{" +
                "domainId='" + domainId + '\'' +
                ", ownerId='" + ownerId + '\'' +
                '}';
    }
}

/**
 * Event class - Represents a message/event in the messaging queue
 */
class Event {
    private String eventId; // "event-123"
    private String eventSource; // "PublisherId-1"
    private EventMetadata eventMetadata;

    public Event(String eventId, String eventSource, EventMetadata eventMetadata) {
        this.eventId = eventId;
        this.eventSource = eventSource;
        this.eventMetadata = eventMetadata;
    }

    public String getEventSource() {
        return eventSource;
    }

    @Override
    public String toString() {
        return "Event{" +
                "eventId='" + eventId + '\'' +
                ", eventSource='" + eventSource + '\'' +
                ", eventMetadata=" + eventMetadata +
                '}';
    }
}

/**
 * Topic class - Manages events, publishers, and subscribers for a specific topic
 * Thread Safety: Uses ConcurrentHashMap and Collections.synchronizedList
 */
class Topic {
    private final List<Event> eventsData; // Thread-safe list to store all events
    private final Map<String, Integer> subscriberOffsets; // {subscriberId, offset}
    private final Set<String> publishers; // Thread-safe set of registered publishers
    private final ExecutorService producerExecutorService; // Async executor for publishing

    public Topic() {
        // Thread-safe collections for concurrent access
        this.eventsData = Collections.synchronizedList(new ArrayList<>());
        this.subscriberOffsets = new ConcurrentHashMap<>();
        this.publishers = ConcurrentHashMap.newKeySet();
        this.producerExecutorService = Executors.newSingleThreadExecutor();
    }

    /**
     * Register a new publisher - Thread-safe operation
     */
    public void addPublisher(String publisherId) {
        this.publishers.add(publisherId);
    }

    /**
     * Check if publisher exists - Thread-safe read operation
     */
    public boolean publisherExists(String publisherId) {
        return this.publishers.contains(publisherId);
    }

    /**
     * Register a new subscriber with offset 0 - Atomic operation using putIfAbsent
     */
    public void addSubscriber(String subscriberId) {
        this.subscriberOffsets.putIfAbsent(subscriberId, 0);
    }

    /**
     * Get next event for subscriber - Synchronized to prevent race conditions on offset updates
     */
    public synchronized Event getEvent(String subscriberId) {
        Integer offset = subscriberOffsets.get(subscriberId); // Get current offset
        if(offset == null || offset >= eventsData.size()) {
            return null; // No new events available
        }
        Event event = eventsData.get(offset); // Get event at current offset
        subscriberOffsets.put(subscriberId, offset+1); // Increment offset for next read
        return event;
    }

    /**
     * Publish event asynchronously - Non-blocking operation
     */
    public void putEvent(Event event) {
        producerExecutorService.submit(() -> {
            this.eventsData.add(event); // Thread-safe add operation
        });
    }
}

/**
 * TopicService class - Core service managing all topics and operations
 */
class TopicService {
    private final Map<String, Topic> topicMap; // Thread-safe storage of topics

    public TopicService() {
        this.topicMap = new ConcurrentHashMap<>();
    }

    /**
     * Register new topic
     */
    public void registerTopic(String topicName) {
        topicMap.put(topicName, new Topic());
    }

    /**
     * Add publisher to topic
     */
    public void addPublisher(String topicName, String publisherId) {
        topicMap.get(topicName).addPublisher(publisherId);
    }

    /**
     * Register subscriber to topic
     */
    public void registerSubscriber(String topicName, String subscriberId) {
        topicMap.get(topicName).addSubscriber(subscriberId);
    }

    /**
     * Publish event with publisher validation
     * Business Rule: Only registered publishers can publish
     */
    public void putEvent(String topicName, Event event) {
        String publisherId = event.getEventSource();
        if(!topicMap.get(topicName).publisherExists(publisherId)) {
            throw new RuntimeException("Publisher : " + publisherId + " can't publish message to the topic : " + topicName);
        }
        topicMap.get(topicName).putEvent(event);
    }

    /**
     * Get next event for subscriber
     */
    public Event getEvent(String topicName, String subscriberId) {
        return topicMap.get(topicName).getEvent(subscriberId);
    }
}

/**
 * MessageQueueSystem - Main demonstration class
 */
public class MessageQueueSystem {

    public static void main(String[] args) {
        System.out.println("Messaging Queue!!");

        // Initialize messaging system
        TopicService topicService = new TopicService();

        // Register topic
        topicService.registerTopic("topic-orders");

        // Create sample events
        Event event1 = new Event("eventId-123", "publisherId-1", new EventMetadata("orderId-123", "userId-123"));
        Event event2 = new Event("eventId-124", "publisherId-2", new EventMetadata("orderId-124", "userId-124"));
        Event event3 = new Event("eventId-125", "publisherId-3", new EventMetadata("orderId-125", "userId-125"));

        // Try to publish without registered publisher (should fail)
        try {
            topicService.putEvent("topic-orders", event1);
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }

        // Register publishers
        topicService.addPublisher("topic-orders", "publisherId-1");
        topicService.addPublisher("topic-orders", "publisherId-2");
        topicService.addPublisher("topic-orders", "publisherId-3");

        // Publish events (processed asynchronously)
        topicService.putEvent("topic-orders", event1);
        topicService.putEvent("topic-orders", event2);
        topicService.putEvent("topic-orders", event3);

        // Wait for async publishing to complete
        try {
            Thread.sleep(100); // Allow time for async operations
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        // Register subscriber
        topicService.registerSubscriber("topic-orders", "subscriberId1");

        // Demonstrate offset-based consumption
        logEvent(topicService.getEvent("topic-orders","subscriberId1")); // Gets event1
        logEvent(topicService.getEvent("topic-orders","subscriberId1")); // Gets event2
        logEvent(topicService.getEvent("topic-orders","subscriberId1")); // Gets event3
        logEvent(topicService.getEvent("topic-orders","subscriberId1")); // Gets null (no more events)
    }

    /**
     * Utility method to safely log events
     */
    public static void logEvent(Event event) {
        if(event == null) {
            System.out.println("Event is null");
            return;
        }
        System.out.println(event);
    }
}

/*
 * ===========================================================================================
 * MULTI-THREADED PUBLISHER-SUBSCRIBER MESSAGE QUEUE SYSTEM - MINIMAL INTERVIEW VERSION
 * ===========================================================================================
 * 
 * PROBLEM STATEMENT:
 * Design and implement a multi-threaded publisher-subscriber message queue system in Java.
 * Multiple publishers send messages concurrently while multiple subscribers consume them.
 * 
 * CORE REQUIREMENTS IMPLEMENTED:
 * 1. Multiple Publishers: Concurrent publishing using ExecutorService
 * 2. Multiple Subscribers: Independent offset tracking per subscriber
 * 3. Thread Safety: ConcurrentHashMap, synchronized methods, thread-safe collections
 * 4. Message Ordering: Offset-based consumption maintains order per subscriber
 * 5. Topic-based: Publishers send to topics, subscribers subscribe to topics
 * 
 * ESSENTIAL FUNCTIONS ONLY:
 * - registerTopic(): Create new topic
 * - addPublisher(): Register publisher to topic
 * - registerSubscriber(): Register subscriber to topic
 * - putEvent(): Publish event with validation
 * - getEvent(): Consume event with offset tracking
 * 
 * THREAD SAFETY MECHANISMS:
 * 1. ConcurrentHashMap: For topicMap, subscriberOffsets, publishers
 * 2. Collections.synchronizedList: For thread-safe event storage
 * 3. synchronized getEvent(): Prevents race conditions on offset updates
 * 4. ExecutorService: Asynchronous non-blocking event publishing
 * 5. Atomic putIfAbsent(): Thread-safe subscriber registration
 * 
 * KEY DESIGN DECISIONS:
 * - Single-threaded executor per topic maintains event ordering
 * - Independent subscriber offsets enable parallel consumption
 * - Publisher validation enforces access control
 * - Minimal functions for interview time constraints
 * ===========================================================================================
 */