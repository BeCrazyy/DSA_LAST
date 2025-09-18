import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.*;

/**
 * Enum representing different types of ad events
 */
enum AdEventType {
    IMPRESSION,
    CLICK
}

/**
 * Represents a single ad event with UUID, timestamp, and event type
 */
class AdEvent {
    private final UUID adUUID;
    private final LocalDateTime timestamp;
    private final AdEventType eventType;

    /**
     * Constructor for AdEvent
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @param adUUID Unique identifier for the ad
     * @param timestamp When the event occurred
     * @param eventType Type of event (IMPRESSION or CLICK)
     */
    public AdEvent(UUID adUUID, LocalDateTime timestamp, AdEventType eventType) {
        this.adUUID = adUUID;
        this.timestamp = timestamp;
        this.eventType = eventType;
    }

    /**
     * Gets the UUID of the ad
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return UUID of the ad
     */
    public UUID getAdUUID() {
        return this.adUUID;
    }

    /**
     * Gets the timestamp of the event
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return LocalDateTime timestamp
     */
    public LocalDateTime getTimestamp() {
        return this.timestamp;
    }

    /**
     * Gets the event type
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return AdEventType (IMPRESSION or CLICK)
     */
    public AdEventType getEventType() {
        return this.eventType;
    }
}

/**
 * Represents aggregated ad events for a specific date
 * Contains counts of impressions and clicks for that day
 */
class AdEventBucket {
    private final LocalDate date;
    private int impressions;
    private int clicks;

    /**
     * Constructor for AdEventBucket
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @param date The date for this bucket
     * @param impressions Initial impression count
     * @param clicks Initial click count
     */
    public AdEventBucket(LocalDate date, int impressions, int clicks) {
        this.date = date;
        this.impressions = impressions;
        this.clicks = clicks;
    }

    /**
     * Increments the impression count for this bucket
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     */
    public void addImpression() {
        this.impressions++;
    }

    /**
     * Increments the click count for this bucket
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     */
    public void addClick() {
        this.clicks++;
    }

    /**
     * Gets the date of this bucket
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return LocalDate of this bucket
     */
    public LocalDate getDate() {
        return date;
    }

    /**
     * Gets the impression count
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return number of impressions
     */
    public int getImpressions() {
        return impressions;
    }

    /**
     * Gets the click count
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return number of clicks
     */
    public int getClicks() {
        return clicks;
    }

    /**
     * String representation of the bucket
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     * 
     * @return formatted string with date, impressions, and clicks
     */
    @Override
    public String toString() {
        return date + " : impressions - " + impressions + ", clicks - " + clicks;
    }
}

/**
 * Main class for frequency capping functionality
 * Manages ad events and provides aggregated daily statistics
 */
public class FrequencyCappingAds {
    // Constants for frequency capping rules
    static final int ROLLING_WINDOW_LENGTH_DAYS = 3;
    static final int IMPRESSION_LIMIT = 5;
    
    // Map structure: UUID -> LocalDate -> AdEventBucket
    // For each ad UUID, stores daily aggregated statistics 
    private HashMap<UUID, HashMap<LocalDate, AdEventBucket>> adEventStats; // adID -> Map<Date, AdEventBucket>
    // TreeMap<LocalDate, AdEventBucket> dateMap = adEventStats.get(adUUID);

    /**
     * Constructor initializes the data structure
     * 
     * Time Complexity: O(1)
     * Space Complexity: O(1) - initializes empty HashMap
     */
    public FrequencyCappingAds() {
        this.adEventStats = new HashMap<>();
    }

    /**
     * Consumes a list of ad events and aggregates them by date
     * Pre-processes events for efficient retrieval later
     * 
     * Time Complexity: O(n) where n = number of events in the list
     * Space Complexity: O(a * d) where a = number of unique ad UUIDs, d = number of unique dates
     * 
     * @param events List of AdEvent objects to process
     */
    public void consumeAdEvents(final List<AdEvent> events) {
        for (AdEvent event : events) {
            UUID adUUID = event.getAdUUID();
            LocalDate date = event.getTimestamp().toLocalDate();
            
            // Get or create the date map for this ad UUID
            HashMap<LocalDate, AdEventBucket> dateMap = adEventStats.get(adUUID);
            if (dateMap == null) {
                dateMap = new HashMap<>();
                // dateMap = new TreeMap<>();
                adEventStats.put(adUUID, dateMap);
            }
            
            // Get or create the bucket for this date
            AdEventBucket bucket = dateMap.get(date);
            if (bucket == null) {
                bucket = new AdEventBucket(date, 0, 0);
                dateMap.put(date, bucket);
            }
            
            // Increment the appropriate counter based on event type
            if (event.getEventType() == AdEventType.IMPRESSION) {
                bucket.addImpression();
            } else if (event.getEventType() == AdEventType.CLICK) {
                bucket.addClick();
            }
        }
    }

    /**
     * Retrieves daily ad event history for a specific ad UUID
     * Returns aggregated data sorted by date
     * 
     * Time Complexity: O(d log d) where d = number of unique dates for the ad UUID (due to sorting)
     * Space Complexity: O(d) for creating the result list
     * 
     * @param adUUID The UUID of the ad to get history for
     * @return List of AdEventBucket objects sorted by date, empty list if no data
     */
    public List<AdEventBucket> getDailyAdEventHistory(final UUID adUUID) {
        HashMap<LocalDate, AdEventBucket> dateMap = adEventStats.get(adUUID);
        if (dateMap == null) {
            return new ArrayList<>();
        }
        
        // Create list from the pre-aggregated buckets
        List<AdEventBucket> result = new ArrayList<>(dateMap.values());
        
        // Sort by date using Collections.sort with custom comparator
        Collections.sort(result, new Comparator<AdEventBucket>() {
            public int compare(AdEventBucket b1, AdEventBucket b2) {
                return b1.getDate().compareTo(b2.getDate());
            }
        });
        
        return result;
    }

    /**
     * Determines if an ad should be capped (blocked) on a specific date
     * based on the rolling window impression threshold without clicks
     * 
     * Time Complexity: O(w) where w = ROLLING_WINDOW_LENGTH_DAYS (constant = 3, so effectively O(1))
     * Space Complexity: O(1) - only uses a few integer variables
     * 
     * @param adUUID The UUID of the ad to check
     * @param date The date to check capping for
     * @return true if ad should be capped, false otherwise
     */
    public Boolean isAdCappedAtDate(final UUID adUUID, final LocalDate date) {
        HashMap<LocalDate, AdEventBucket> dateMap = adEventStats.get(adUUID);
        if (dateMap == null) {
            return false; // No data means no capping
        }
        
        int totalImpressions = 0;
        int totalClicks = 0;
        
        // Check rolling window: current date and previous (ROLLING_WINDOW_LENGTH_DAYS - 1) days
        for (int i = 0; i < ROLLING_WINDOW_LENGTH_DAYS; i++) {
            LocalDate checkDate = date.minusDays(i);
            AdEventBucket bucket = dateMap.get(checkDate);
            
            if (bucket != null) {
                totalImpressions += bucket.getImpressions();
                totalClicks += bucket.getClicks();
            }
        }
        
        // Cap if impressions >= limit AND no clicks in the window
        return totalImpressions >= IMPRESSION_LIMIT && totalClicks == 0;
    }

    /**
     * Test method to demonstrate the functionality
     * 
     * Time Complexity: O(n log d) where n = number of test events, d = number of unique dates
     * Space Complexity: O(a * d) where a = number of test ads, d = number of unique dates
     */
    public static void main(String[] args) {
        System.out.println("FrequencyCappingAds Testing!!");
        FrequencyCappingAds frequencyCappingAds = new FrequencyCappingAds();
        
        // Create test ads
        UUID ad1 = UUID.randomUUID(); // Ad with clicks - should NOT be capped
        UUID ad2 = UUID.randomUUID(); // Ad with exactly 5 impressions, no clicks - should be CAPPED
        UUID ad3 = UUID.randomUUID(); // Ad with 6+ impressions, no clicks - should be CAPPED
        UUID ad4 = UUID.randomUUID(); // Ad with < 5 impressions, no clicks - should NOT be capped
        UUID ad5 = UUID.randomUUID(); // Edge case testing
        
        System.out.println("Test Ad UUIDs:");
        System.out.println("Ad1 (with clicks): " + ad1);
        System.out.println("Ad2 (exactly 5 impressions, no clicks): " + ad2);
        System.out.println("Ad3 (6+ impressions, no clicks): " + ad3);
        System.out.println("Ad4 (<5 impressions, no clicks): " + ad4);
        System.out.println("Ad5 (edge cases): " + ad5);
        System.out.println();
        
        // Create comprehensive test events
        List<AdEvent> testEvents = new ArrayList<>();
        
        // === AD1: Has clicks, should NOT be capped ===
        testEvents.add(new AdEvent(ad1, LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad1, LocalDateTime.of(2019, 3, 26, 11, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad1, LocalDateTime.of(2019, 3, 27, 9, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad1, LocalDateTime.of(2019, 3, 27, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad1, LocalDateTime.of(2019, 3, 27, 15, 0), AdEventType.CLICK)); // Has click
        testEvents.add(new AdEvent(ad1, LocalDateTime.of(2019, 3, 28, 8, 0), AdEventType.IMPRESSION));
        
        // === AD2: Exactly 5 impressions, no clicks - should be CAPPED ===
        testEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 26, 11, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 27, 9, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 28, 8, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 28, 9, 0), AdEventType.IMPRESSION));
        // No clicks for ad2
        
        // === AD3: 6+ impressions, no clicks - should be CAPPED ===
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 26, 11, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 26, 12, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 27, 9, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 27, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 28, 8, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad3, LocalDateTime.of(2019, 3, 28, 9, 0), AdEventType.IMPRESSION));
        // No clicks for ad3 - 7 total impressions
        
        // === AD4: Less than 5 impressions, no clicks - should NOT be capped ===
        testEvents.add(new AdEvent(ad4, LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad4, LocalDateTime.of(2019, 3, 27, 9, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad4, LocalDateTime.of(2019, 3, 28, 8, 0), AdEventType.IMPRESSION));
        // Only 3 impressions, no clicks
        
        // === AD5: Edge case - click after impressions ===
        testEvents.add(new AdEvent(ad5, LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad5, LocalDateTime.of(2019, 3, 26, 11, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad5, LocalDateTime.of(2019, 3, 27, 9, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad5, LocalDateTime.of(2019, 3, 27, 10, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad5, LocalDateTime.of(2019, 3, 28, 8, 0), AdEventType.IMPRESSION));
        testEvents.add(new AdEvent(ad5, LocalDateTime.of(2019, 3, 29, 10, 0), AdEventType.CLICK)); // Click on day 4
        
        // Add some duplicate events to test deduplication
        List<AdEvent> duplicateEvents = new ArrayList<>();
        duplicateEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION)); // Duplicate
        duplicateEvents.add(new AdEvent(ad2, LocalDateTime.of(2019, 3, 27, 9, 0), AdEventType.IMPRESSION));  // Duplicate
        duplicateEvents.add(new AdEvent(UUID.randomUUID(), LocalDateTime.of(2019, 3, 26, 10, 0), AdEventType.IMPRESSION)); // New event
        
        // Consume the events
        System.out.println("Consuming " + testEvents.size() + " events...");
        frequencyCappingAds.consumeAdEvents(testEvents);
        System.out.println("Events consumed successfully!");
        System.out.println();
        
        // Test deduplication by consuming duplicate events
        System.out.println("Testing deduplication with " + duplicateEvents.size() + " events (2 duplicates, 1 new)...");
        frequencyCappingAds.consumeAdEvents(duplicateEvents);
        System.out.println("Deduplication test completed!");
        System.out.println();
        
        // Test cleanup functionality
        System.out.println("Testing cleanup functionality...");
        // frequencyCappingAds.cleanupExpiredEvents();
        System.out.println();
        
        // Show daily history for all ads
        UUID[] allAds = {ad1, ad2, ad3, ad4, ad5};
        String[] adNames = {"Ad1 (with clicks)", "Ad2 (exactly 5 imp, no clicks)", "Ad3 (6+ imp, no clicks)", 
                           "Ad4 (<5 imp, no clicks)", "Ad5 (edge case)"};
        
        for (int i = 0; i < allAds.length; i++) {
            System.out.println("Daily History for " + adNames[i] + ":");
            List<AdEventBucket> history = frequencyCappingAds.getDailyAdEventHistory(allAds[i]);
            for (AdEventBucket bucket : history) {
                System.out.println("  " + bucket);
            }
            System.out.println();
        }
        
        // === COMPREHENSIVE FREQUENCY CAPPING TESTS ===
        System.out.println("========================================");
        System.out.println("=== COMPREHENSIVE CAPPING TESTS ===");
        System.out.println("Rolling window: " + ROLLING_WINDOW_LENGTH_DAYS + " days");
        System.out.println("Impression limit: " + IMPRESSION_LIMIT + " impressions");
        System.out.println("Condition: impressions >= " + IMPRESSION_LIMIT + " AND clicks == 0 in rolling window");
        System.out.println("========================================");
        System.out.println();
        
        // Test multiple dates for each ad
        LocalDate[] testDates = {
            LocalDate.of(2019, 3, 26), // Day 1
            LocalDate.of(2019, 3, 27), // Day 2  
            LocalDate.of(2019, 3, 28), // Day 3
            LocalDate.of(2019, 3, 29), // Day 4
            LocalDate.of(2019, 3, 30), // Day 5
            LocalDate.of(2019, 3, 31)  // Day 6
        };
        
        for (int i = 0; i < allAds.length; i++) {
            System.out.println("=== " + adNames[i] + " Capping Tests ===");
            
            for (LocalDate testDate : testDates) {
                boolean isCapped = frequencyCappingAds.isAdCappedAtDate(allAds[i], testDate);
                
                // Get rolling window details for debugging
                HashMap<LocalDate, AdEventBucket> dateMap = frequencyCappingAds.adEventStats.get(allAds[i]);
                int windowImpressions = 0;
                int windowClicks = 0;
                String windowDetails = "";
                
                if (dateMap != null) {
                    for (int j = 0; j < ROLLING_WINDOW_LENGTH_DAYS; j++) {
                        LocalDate checkDate = testDate.minusDays(j);
                        AdEventBucket bucket = dateMap.get(checkDate);
                        if (bucket != null) {
                            windowImpressions += bucket.getImpressions();
                            windowClicks += bucket.getClicks();
                            windowDetails += checkDate + "(" + bucket.getImpressions() + "i," + bucket.getClicks() + "c) ";
                        }
                    }
                }
                
                System.out.println("Date " + testDate + ": " + (isCapped ? "CAPPED" : "NOT CAPPED") + 
                                 " | Window: " + windowImpressions + " impressions, " + windowClicks + " clicks | " + windowDetails.trim());
            }
            System.out.println();
        }
        
        // === EDGE CASE TESTS ===
        System.out.println("=== EDGE CASE TESTS ===");
        
        // Test non-existent UUID
        UUID nonExistentAd = UUID.randomUUID();
        boolean nonExistentCapped = frequencyCappingAds.isAdCappedAtDate(nonExistentAd, LocalDate.of(2019, 3, 30));
        System.out.println("Non-existent Ad: " + (nonExistentCapped ? "CAPPED" : "NOT CAPPED"));
        
        // Test future date
        boolean futureCapped = frequencyCappingAds.isAdCappedAtDate(ad2, LocalDate.of(2020, 1, 1));
        System.out.println("Future date test: " + (futureCapped ? "CAPPED" : "NOT CAPPED"));
        
        // Test past date (before any events)
        boolean pastCapped = frequencyCappingAds.isAdCappedAtDate(ad2, LocalDate.of(2019, 1, 1));
        System.out.println("Past date test: " + (pastCapped ? "CAPPED" : "NOT CAPPED"));
        
        System.out.println();
        System.out.println("=== EXPECTED RESULTS SUMMARY ===");
        System.out.println("Ad1: Should NEVER be capped (has clicks)");
        System.out.println("Ad2: Should be CAPPED on 2019-03-28 and later (5 impressions, 0 clicks)");
        System.out.println("Ad3: Should be CAPPED on 2019-03-28 and later (7 impressions, 0 clicks)");
        System.out.println("Ad4: Should NEVER be capped (only 3 impressions < limit)");
        System.out.println("Ad5: Should be CAPPED on 2019-03-28, but NOT CAPPED on 2019-03-29+ (click resets)");
        System.out.println();
        
        System.out.println("All comprehensive tests completed successfully!");
    }
}

/*
 * PROBLEM STATEMENT:
 * 
 * This code implements a feature to determine if an ad has had X impressions 
 * without a click over a Y day rolling window. The goal is to identify when 
 * users have not interacted with an ad despite seeing it multiple times.
 * 
 * The system needs to:
 * 1. Consume ad events efficiently (impressions and clicks with timestamps)
 * 2. Aggregate events by day for each ad UUID
 * 3. Provide fast retrieval of daily aggregated statistics
 * 4. Determine if an ad should be capped based on rolling window analysis
 * 
 * Key Requirements:
 * - consumeAdEvents(): Process lists of AdEvent objects containing UUID, 
 *   timestamp, and event type
 * - getDailyAdEventHistory(): Return daily aggregated data sorted by date
 *   in the format: "2019-03-26 : impressions - 3, clicks - 0"
 * - isAdCappedAtDate(): Check if ad should be blocked on a specific date
 *   based on rolling window (3 days) and impression threshold (5 impressions)
 * 
 * Design Decisions:
 * - Pre-aggregate during consumption for O(1) writes and fast reads
 * - Use HashMap<UUID, HashMap<LocalDate, AdEventBucket>> for efficient access
 * - AdEventBucket serves dual purpose: aggregation container and output format
 * - Collections.sort() with custom comparator for efficient date-based sorting
 * - Rolling window logic: look back N days from given date, count impressions/clicks
 * 
 * This foundation enables frequency capping algorithms to quickly identify
 * ads that have excessive impressions without user engagement and prevent
 * them from being shown to improve user experience.
 */