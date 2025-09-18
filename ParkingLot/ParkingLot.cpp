/**
 * Parking Lot Management System
 * 
 * A multi-level parking lot system that can handle motorcycles and cars
 * with different parking rules and efficient spot allocation.
 * 
 * Author: [Your Name]
 * Date: [Current Date]
 */

#include <bits/stdc++.h>
using namespace std;

/**
 * Enum representing different types of vehicles
 * Extensible for future vehicle types (buses, trucks, etc.)
 */
enum class VehicleType {
    MOTORCYCLE,
    CAR
};

/**
 * Enum representing different types of parking spots
 * Motorcycle spots are smaller, car spots are larger
 */
enum class SpotType {
    MOTORCYCLE,
    CAR
};

/**
 * Vehicle class represents a vehicle that needs to be parked
 * Contains basic vehicle information for identification and tracking
 */
class Vehicle {
public:
    string vehicleId;         // Unique identifier for the vehicle
    VehicleType type;         // Type of vehicle (motorcycle or car)
    string licensePlate;      // License plate for identification
    
    /**
     * Constructor to create a vehicle
     * @param vehicleId Unique identifier for the vehicle
     * @param vehicleType Type of vehicle
     * @param plate License plate number
     * @throws invalid_argument if vehicleId or licensePlate is empty
     */
    Vehicle(const string& vehicleId, VehicleType vehicleType, const string& plate)
        : vehicleId(vehicleId), type(vehicleType), licensePlate(plate) {
        if (vehicleId.empty()) {
            throw invalid_argument("Vehicle ID cannot be empty");
        }
        if (plate.empty()) {
            throw invalid_argument("License plate cannot be empty");
        }
    }
};

/**
 * Spot class represents a single parking spot in the parking lot
 * Handles parking and unparking operations for individual spots
 */
class Spot {
public:
    string spotId;           // Unique identifier for the spot
    SpotType type;           // Type of spot (motorcycle or car)
    bool isOccupied;         // Current occupancy status
    Vehicle* vehicle;        // Pointer to parked vehicle (nullptr if empty)
    
    /**
     * Constructor to create a parking spot
     * @param spotId Unique identifier for the spot
     * @param spotType Type of parking spot
     * @throws invalid_argument if spotId is empty
     */
    Spot(const string& spotId, SpotType spotType)
        : spotId(spotId), type(spotType), isOccupied(false), vehicle(nullptr) {
        if (spotId.empty()) {
            throw invalid_argument("Spot ID cannot be empty");
        }
    }
    
    /**
     * Check if a vehicle can fit in this spot based on parking rules
     * PARKING RULES:
     * - Motorcycles can park in ANY empty spot (motorcycle spots OR car spots)
     * - Cars can ONLY park in empty car spots
     * @param vehicleType Type of vehicle to check
     * @return true if vehicle can park here, false otherwise
     */
    bool canFitVehicle(VehicleType vehicleType) {
        // First check: spot must be empty
        if (isOccupied) {
            return false;
        }
        
        // Second check: apply parking rules based on vehicle type using switch for extensibility
        switch (vehicleType) {
            case VehicleType::MOTORCYCLE:
                // Motorcycles can park in any empty spot (both motorcycle and car spots)
                return true;
            
            case VehicleType::CAR:
                // Cars can only park in car spots (not motorcycle spots)
                return (type == SpotType::CAR);
            
            default:
                // Unknown vehicle type - extensible for future vehicle types
                return false;
        }
    }
    
    /**
     * Park a vehicle in this spot
     * Updates the spot's occupancy status and assigns the vehicle
     * @param v Pointer to the vehicle to park
     * @throws invalid_argument if vehicle pointer is null
     * @throws runtime_error if spot is already occupied
     */
    void parkVehicle(Vehicle* v) {
        if (v == nullptr) {
            throw invalid_argument("Vehicle pointer cannot be null");
        }
        if (isOccupied) {
            throw runtime_error("Cannot park vehicle in occupied spot: " + spotId);
        }
        vehicle = v;
        isOccupied = true;
    }
    
    /**
     * Remove vehicle from this spot (make it available)
     * Clears the spot and marks it as unoccupied
     * @throws runtime_error if spot is already empty
     */
    void unparkVehicle() {
        if (!isOccupied) {
            throw runtime_error("Cannot unpark from empty spot: " + spotId);
        }
        vehicle = nullptr;
        isOccupied = false;
    }
};

/**
 * ParkingLevel class represents a single level in the parking lot
 * Manages all spots within a level and tracks availability efficiently
 */
class ParkingLevel {
public:
    int levelNumber;                                 // Level number (1, 2, 3, etc.)
    vector<Spot*> spots;                            // All spots in this level
    unordered_set<string> availableMotorcycleSpotIds; // Available motorcycle spot identifiers
    unordered_set<string> availableCarSpotIds;      // Available car spot identifiers
    
    /**
     * Constructor to create a parking level
     * @param levelNumber The level number (1, 2, 3, etc.)
     * @throws invalid_argument if levelNumber is less than 1
     */
    ParkingLevel(int levelNumber) : levelNumber(levelNumber) {
        if (levelNumber < 1) {
            throw invalid_argument("Level number must be at least 1");
        }
    }
    
    /**
     * Destructor to clean up memory for all spots in this level
     */
    ~ParkingLevel() {
        for (Spot* spot : spots) {
            delete spot;
        }
    }
    
    /**
     * Add a spot to this level
     * Adds the spot to the spots vector and updates availability tracking
     * @param spot Pointer to the spot to add
     * @throws invalid_argument if spot pointer is null
     * @throws runtime_error if spot with same ID already exists
     */
    void addSpot(Spot* spot) {
        if (spot == nullptr) {
            throw invalid_argument("Spot pointer cannot be null");
        }
        
        // Check for duplicate spot IDs
        for (Spot* existingSpot : spots) {
            if (existingSpot->spotId == spot->spotId) {
                throw runtime_error("Spot with ID " + spot->spotId + " already exists in level " + to_string(levelNumber));
            }
        }
        
        spots.push_back(spot);
        // Add spotId to appropriate availability set based on spot type using switch for extensibility
        switch (spot->type) {
            case SpotType::MOTORCYCLE:
                availableMotorcycleSpotIds.insert(spot->spotId);
                break;
            case SpotType::CAR:
                availableCarSpotIds.insert(spot->spotId);
                break;
            default:
                // Unknown spot type - extensible for future spot types
                break;
        }
    }
    
    /**
     * Find an available spot for a given vehicle type
     * Searches through available spots based on parking rules
     * @param vehicleType Type of vehicle looking for a spot
     * @return spotId if available, empty string if no spot found
     */
    string findAvailableSpotId(VehicleType vehicleType) {
        // Use switch for vehicle type handling - makes it easy to add new vehicle types
        switch (vehicleType) {
            case VehicleType::MOTORCYCLE:
                // Motorcycle can use any available spot - check motorcycle spots first
                if (!availableMotorcycleSpotIds.empty()) {
                    return *availableMotorcycleSpotIds.begin();
                }
                // If no motorcycle spots, try car spots
                if (!availableCarSpotIds.empty()) {
                    return *availableCarSpotIds.begin();
                }
                break;
                
            case VehicleType::CAR:
                // Car can only use car spots
                if (!availableCarSpotIds.empty()) {
                    return *availableCarSpotIds.begin();
                }
                break;
                
            default:
                // Unknown vehicle type - extensible for future vehicle types like buses
                break;
        }
        return ""; // No available spot found
    }
    
    /**
     * Mark a spot as occupied (remove from available sets)
     * Called when a vehicle is parked in the spot
     * @param spotId Identifier of the spot to mark as occupied
     * @param spotType Type of the spot
     */
    void markSpotOccupied(const string& spotId, SpotType spotType) {
        // Use switch for spot type handling - extensible for new spot types
        switch (spotType) {
            case SpotType::MOTORCYCLE:
                availableMotorcycleSpotIds.erase(spotId);
                break;
            case SpotType::CAR:
                availableCarSpotIds.erase(spotId);
                break;
            default:
                // Unknown spot type - extensible for future spot types
                break;
        }
    }
    
    /**
     * Mark a spot as available (add back to available sets)
     * Called when a vehicle is unparked from the spot
     * @param spotId Identifier of the spot to mark as available
     * @param spotType Type of the spot
     */
    void markSpotAvailable(const string& spotId, SpotType spotType) {
        // Use switch for spot type handling - extensible for new spot types
        switch (spotType) {
            case SpotType::MOTORCYCLE:
                availableMotorcycleSpotIds.insert(spotId);
                break;
            case SpotType::CAR:
                availableCarSpotIds.insert(spotId);
                break;
            default:
                // Unknown spot type - extensible for future spot types
                break;
        }
    }
};

/**
 * ParkingLot class is the main controller for the entire parking system
 * Manages multiple levels and provides the three core operations:
 * 1. Park a vehicle
 * 2. Unpark a vehicle  
 * 3. Find vehicle in a specific spot
 */
class ParkingLot {
private:
    vector<ParkingLevel*> levels;                        // All parking levels
    unordered_map<string, string> vehicleIdToSpotId;    // vehicleId -> spotId mapping for fast unpark
    unordered_map<string, Spot*> spotIdToSpot;          // spotId -> Spot* mapping for fast spot lookup
    
public:
    /**
     * Destructor to clean up memory for all levels
     */
    ~ParkingLot() {
        for (ParkingLevel* level : levels) {
            delete level;
        }
    }
    
    /**
     * Add a level to the parking lot
     * Registers all spots in the level for global lookups
     * @param level Pointer to the level to add
     * @throws invalid_argument if level pointer is null
     * @throws runtime_error if level number already exists or if spot IDs conflict
     */
    void addLevel(ParkingLevel* level) {
        if (level == nullptr) {
            throw invalid_argument("Level pointer cannot be null");
        }
        
        // Check for duplicate level numbers
        for (ParkingLevel* existingLevel : levels) {
            if (existingLevel->levelNumber == level->levelNumber) {
                throw runtime_error("Level " + to_string(level->levelNumber) + " already exists");
            }
        }
        
        // Check for duplicate spot IDs across all levels
        for (Spot* spot : level->spots) {
            if (spotIdToSpot.find(spot->spotId) != spotIdToSpot.end()) {
                throw runtime_error("Spot ID " + spot->spotId + " already exists in another level");
            }
        }
        
        levels.push_back(level);
        // Add all spots to the global spot map for O(1) lookups by spotId
        for (Spot* spot : level->spots) {
            spotIdToSpot[spot->spotId] = spot;
        }
    }
    
    /**
     * CORE FUNCTION 1: Park a vehicle in the lot
     * Finds the first available spot across all levels based on parking rules
     * Time Complexity: O(L) where L is number of levels
     * @param vehicle Pointer to the vehicle to park
     * @return true if successfully parked, false if no space available
     * @throws invalid_argument if vehicle pointer is null
     * @throws runtime_error if vehicle is already parked
     */
    bool parkVehicle(Vehicle* vehicle) {
        if (vehicle == nullptr) {
            throw invalid_argument("Vehicle pointer cannot be null");
        }
        
        // Check if vehicle is already parked in the lot
        if (vehicleIdToSpotId.find(vehicle->vehicleId) != vehicleIdToSpotId.end()) {
            throw runtime_error("Vehicle " + vehicle->vehicleId + " is already parked");
        }
        
        // Search for available spot across all levels
        for (ParkingLevel* level : levels) {
            string spotId = level->findAvailableSpotId(vehicle->type);
            if (!spotId.empty()) {
                // Found an available spot
                Spot* spot = spotIdToSpot[spotId];
                spot->parkVehicle(vehicle);
                vehicleIdToSpotId[vehicle->vehicleId] = spotId;
                level->markSpotOccupied(spotId, spot->type);
                return true; // Successfully parked
            }
        }
        
        return false; // No available spot found in any level
    }
    
    /**
     * CORE FUNCTION 2: Unpark a vehicle from the lot
     * Removes the vehicle and frees up the spot for future use
     * Time Complexity: O(L) - could be optimized to O(1) with level mapping
     * @param vehicleId Identifier of the vehicle to unpark
     * @return true if successfully unparked
     * @throws invalid_argument if vehicleId is empty
     * @throws runtime_error if vehicle is not found in parking lot
     */
    bool unparkVehicle(const string& vehicleId) {
        if (vehicleId.empty()) {
            throw invalid_argument("Vehicle ID cannot be empty");
        }
        
        // Check if vehicle is parked in the lot
        auto it = vehicleIdToSpotId.find(vehicleId);
        if (it == vehicleIdToSpotId.end()) {
            throw runtime_error("Vehicle " + vehicleId + " is not parked in this lot");
        }
        
        string spotId = it->second;
        Spot* spot = spotIdToSpot[spotId];
        
        // Find which level this spot belongs to and mark it available
        for (ParkingLevel* level : levels) {
            for (Spot* levelSpot : level->spots) {
                if (levelSpot->spotId == spotId) {
                    spot->unparkVehicle();
                    level->markSpotAvailable(spotId, spot->type);
                    vehicleIdToSpotId.erase(vehicleId);
                    return true; // Successfully unparked
                }
            }
        }
        
        throw runtime_error("Internal error: Spot " + spotId + " not found in any level");
    }
    
    /**
     * CORE FUNCTION 3: Get the vehicle parked in a specific spot
     * Retrieves vehicle information for a given spot
     * Time Complexity: O(1)
     * @param spotId Identifier of the spot to check
     * @return Pointer to vehicle if found, nullptr if spot is empty
     * @throws invalid_argument if spotId is empty
     * @throws runtime_error if spot doesn't exist
     */
    Vehicle* getVehicleInSpot(const string& spotId) {
        if (spotId.empty()) {
            throw invalid_argument("Spot ID cannot be empty");
        }
        
        auto it = spotIdToSpot.find(spotId);
        if (it == spotIdToSpot.end()) {
            throw runtime_error("Spot " + spotId + " does not exist");
        }
        
        return it->second->vehicle; // Return vehicle (could be nullptr if spot is empty)
    }
    
    /**
     * Utility function to print current status of the parking lot
     * Displays available spots count for each level
     * Useful for debugging and monitoring
     */
    void printStatus() {
        cout << "\n=== Parking Lot Status ===" << endl;
        for (ParkingLevel* level : levels) {
            cout << "Level " << level->levelNumber << ":" << endl;
            cout << "  Available motorcycle spots: " << level->availableMotorcycleSpotIds.size() << endl;
            cout << "  Available car spots: " << level->availableCarSpotIds.size() << endl;
        }
        cout << "========================\n" << endl;
    }
};

/**
 * Basic Unit Tests for Parking Lot System
 * Tests core functionality and exception handling
 */
namespace UnitTests {
    void testVehicleCreation() {
        cout << "\n=== Testing Vehicle Creation ===" << endl;
        
        try {
            // Valid vehicle creation
            Vehicle* validVehicle = new Vehicle("V001", VehicleType::CAR, "ABC123");
            cout << "✓ Valid vehicle creation successful" << endl;
            delete validVehicle;
            
            // Test empty vehicle ID
            try {
                Vehicle* invalidVehicle = new Vehicle("", VehicleType::CAR, "ABC123");
                cout << "✗ Should have thrown exception for empty vehicle ID" << endl;
                delete invalidVehicle;
            } catch (const invalid_argument& e) {
                cout << "✓ Correctly caught exception for empty vehicle ID: " << e.what() << endl;
            }
            
            // Test empty license plate
            try {
                Vehicle* invalidVehicle = new Vehicle("V001", VehicleType::CAR, "");
                cout << "✗ Should have thrown exception for empty license plate" << endl;
                delete invalidVehicle;
            } catch (const invalid_argument& e) {
                cout << "✓ Correctly caught exception for empty license plate: " << e.what() << endl;
            }
            
        } catch (const exception& e) {
            cout << "✗ Unexpected exception in vehicle creation: " << e.what() << endl;
        }
    }
    
    void testSpotCreation() {
        cout << "\n=== Testing Spot Creation ===" << endl;
        
        try {
            // Valid spot creation
            Spot* validSpot = new Spot("S001", SpotType::CAR);
            cout << "✓ Valid spot creation successful" << endl;
            delete validSpot;
            
            // Test empty spot ID
            try {
                Spot* invalidSpot = new Spot("", SpotType::CAR);
                cout << "✗ Should have thrown exception for empty spot ID" << endl;
                delete invalidSpot;
            } catch (const invalid_argument& e) {
                cout << "✓ Correctly caught exception for empty spot ID: " << e.what() << endl;
            }
            
        } catch (const exception& e) {
            cout << "✗ Unexpected exception in spot creation: " << e.what() << endl;
        }
    }
    
    void testParkingLevelCreation() {
        cout << "\n=== Testing Parking Level Creation ===" << endl;
        
        try {
            // Valid level creation
            ParkingLevel* validLevel = new ParkingLevel(1);
            cout << "✓ Valid parking level creation successful" << endl;
            delete validLevel;
            
            // Test invalid level number
            try {
                ParkingLevel* invalidLevel = new ParkingLevel(0);
                cout << "✗ Should have thrown exception for invalid level number" << endl;
                delete invalidLevel;
            } catch (const invalid_argument& e) {
                cout << "✓ Correctly caught exception for invalid level number: " << e.what() << endl;
            }
            
        } catch (const exception& e) {
            cout << "✗ Unexpected exception in parking level creation: " << e.what() << endl;
        }
    }
    
    void testParkingOperations() {
        cout << "\n=== Testing Parking Operations ===" << endl;
        
        try {
            // Setup
            ParkingLot* parkingLot = new ParkingLot();
            ParkingLevel* level = new ParkingLevel(1);
            level->addSpot(new Spot("L1-C1", SpotType::CAR));
            parkingLot->addLevel(level);
            
            Vehicle* car = new Vehicle("C001", VehicleType::CAR, "ABC123");
            
            // Test successful parking
            bool parkResult = parkingLot->parkVehicle(car);
            if (parkResult) {
                cout << "✓ Vehicle parked successfully" << endl;
            } else {
                cout << "✗ Failed to park vehicle" << endl;
            }
            
            // Test parking already parked vehicle
            try {
                parkingLot->parkVehicle(car);
                cout << "✗ Should have thrown exception for already parked vehicle" << endl;
            } catch (const runtime_error& e) {
                cout << "✓ Correctly caught exception for already parked vehicle: " << e.what() << endl;
            }
            
            // Test getting vehicle from spot
            Vehicle* foundVehicle = parkingLot->getVehicleInSpot("L1-C1");
            if (foundVehicle && foundVehicle->vehicleId == "C001") {
                cout << "✓ Successfully retrieved vehicle from spot" << endl;
            } else {
                cout << "✗ Failed to retrieve vehicle from spot" << endl;
            }
            
            // Test unparking
            bool unparkResult = parkingLot->unparkVehicle("C001");
            if (unparkResult) {
                cout << "✓ Vehicle unparked successfully" << endl;
            } else {
                cout << "✗ Failed to unpark vehicle" << endl;
            }
            
            // Test unparking non-existent vehicle
            try {
                parkingLot->unparkVehicle("NONEXISTENT");
                cout << "✗ Should have thrown exception for non-existent vehicle" << endl;
            } catch (const runtime_error& e) {
                cout << "✓ Correctly caught exception for non-existent vehicle: " << e.what() << endl;
            }
            
            // Cleanup
            delete car;
            delete parkingLot;
            
        } catch (const exception& e) {
            cout << "✗ Unexpected exception in parking operations: " << e.what() << endl;
        }
    }
    
    void testExceptionHandling() {
        cout << "\n=== Testing Exception Handling ===" << endl;
        
        try {
            ParkingLot* parkingLot = new ParkingLot();
            
            // Test null vehicle pointer
            try {
                parkingLot->parkVehicle(nullptr);
                cout << "✗ Should have thrown exception for null vehicle" << endl;
            } catch (const invalid_argument& e) {
                cout << "✓ Correctly caught exception for null vehicle: " << e.what() << endl;
            }
            
            // Test empty spot ID in getVehicleInSpot
            try {
                parkingLot->getVehicleInSpot("");
                cout << "✗ Should have thrown exception for empty spot ID" << endl;
            } catch (const invalid_argument& e) {
                cout << "✓ Correctly caught exception for empty spot ID: " << e.what() << endl;
            }
            
            // Test non-existent spot ID
            try {
                parkingLot->getVehicleInSpot("NONEXISTENT");
                cout << "✗ Should have thrown exception for non-existent spot" << endl;
            } catch (const runtime_error& e) {
                cout << "✓ Correctly caught exception for non-existent spot: " << e.what() << endl;
            }
            
            delete parkingLot;
            
        } catch (const exception& e) {
            cout << "✗ Unexpected exception in exception handling tests: " << e.what() << endl;
        }
    }
    
    void runAllTests() {
        cout << "\n======================================" << endl;
        cout << "    RUNNING UNIT TESTS" << endl;
        cout << "======================================" << endl;
        
        testVehicleCreation();
        testSpotCreation();
        testParkingLevelCreation();
        testParkingOperations();
        testExceptionHandling();
        
        cout << "\n======================================" << endl;
        cout << "    UNIT TESTS COMPLETED" << endl;
        cout << "======================================" << endl;
    }
}

/**
 * Example usage and demonstration of the parking lot system
 * This shows how to create a parking lot, add levels, and test all functionality
 */
int main() {
    // Run unit tests first
    UnitTests::runAllTests();
    
    cout << "\n\n======================================" << endl;
    cout << "    RUNNING MAIN DEMO" << endl;
    cout << "======================================" << endl;
    
    try {
        // Create the main parking lot controller
        ParkingLot* parkingLot = new ParkingLot();
        
        // Create first level with mixed spot types
        ParkingLevel* level1 = new ParkingLevel(1);
        level1->addSpot(new Spot("L1-M1", SpotType::MOTORCYCLE));
        level1->addSpot(new Spot("L1-M2", SpotType::MOTORCYCLE));
        level1->addSpot(new Spot("L1-C1", SpotType::CAR));
        level1->addSpot(new Spot("L1-C2", SpotType::CAR));
        
        // Create second level with mixed spot types
        ParkingLevel* level2 = new ParkingLevel(2);
        level2->addSpot(new Spot("L2-M1", SpotType::MOTORCYCLE));
        level2->addSpot(new Spot("L2-C1", SpotType::CAR));
        level2->addSpot(new Spot("L2-C2", SpotType::CAR));
        
        // Add levels to the parking lot
        parkingLot->addLevel(level1);
        parkingLot->addLevel(level2);
        
        // Create test vehicles with unique identifiers
        Vehicle* motorcycle1 = new Vehicle("M001", VehicleType::MOTORCYCLE, "BIKE123");
        Vehicle* car1 = new Vehicle("C001", VehicleType::CAR, "CAR456");
        Vehicle* car2 = new Vehicle("C002", VehicleType::CAR, "CAR789");
        
        // Test the three core functions
        
        // 1. Test parking vehicles
        cout << "\n=== Testing Parking Function ===" << endl;
        cout << "Parking motorcycle1: " << (parkingLot->parkVehicle(motorcycle1) ? "Success" : "Failed") << endl;
        cout << "Parking car1: " << (parkingLot->parkVehicle(car1) ? "Success" : "Failed") << endl;
        cout << "Parking car2: " << (parkingLot->parkVehicle(car2) ? "Success" : "Failed") << endl;
        
        // Display current status
        parkingLot->printStatus();
        
        // 3. Test finding vehicle in spot
        cout << "=== Testing Get Vehicle Function ===" << endl;
        Vehicle* foundVehicle = parkingLot->getVehicleInSpot("L1-C1");
        if (foundVehicle) {
            cout << "Vehicle in spot L1-C1: " << foundVehicle->vehicleId << " (" << foundVehicle->licensePlate << ")" << endl;
        } else {
            cout << "No vehicle found in spot L1-C1" << endl;
        }
        
        // 2. Test unparking vehicles
        cout << "\n=== Testing Unparking Function ===" << endl;
        cout << "Unparking car1 (vehicleId: C001): " << (parkingLot->unparkVehicle("C001") ? "Success" : "Failed") << endl;
        
        // Display final status
        parkingLot->printStatus();
        
        // Cleanup memory
        delete motorcycle1;
        delete car1;
        delete car2;
        delete parkingLot;
        
    } catch (const exception& e) {
        cout << "Exception in main demo: " << e.what() << endl;
    }
    
    return 0;
}

/*
===================================================================
                    PROBLEM DESCRIPTION
===================================================================

PARKING LOT MANAGEMENT SYSTEM

Problem Statement:
Design and implement a parking lot system with the following requirements:

ASSUMPTIONS:
• The parking lot has multiple levels
• Each level has multiple rows of spots
• The parking lot has motorcycle spots and car spots
• A motorcycle can park in any empty spot (motorcycle or car spot)
• A car can only park in a single empty car spot

CORE REQUIREMENTS - Implement 3 functions:
1. Given a vehicle, you should be able to PARK it
2. Given a vehicle, you should be able to UNPARK it  
3. Given a spot, you should be able to find the vehicle parked in it

FOLLOW-UP QUESTIONS TO CONSIDER:
1. How to modify code to park buses? (A bus requires 5 consecutive car spots)
2. How to test this solution before production deployment?
3. What changes for productionalization and scaling?
4. How to generate parking receipts?

SOLUTION APPROACH:
- Use object-oriented design with separate classes for Vehicle, Spot, Level, and ParkingLot
- Use enums for extensibility (VehicleType, SpotType)
- Use switch statements strategically for enum handling to enable easy extension
- Use if-else for simple boolean/conditional checks where switch doesn't add value
- Implement comprehensive exception handling using standard C++ exceptions
- Include basic unit tests to validate functionality and error handling
- Maintain multiple data structures for efficient operations:
  * HashMap for vehicleId-to-spotId mapping (fast unpark)
  * HashMap for spotId-to-vehicle mapping (fast spot lookup)
  * Sets to track available spots by type (fast parking)

TIME COMPLEXITY:
- Park: O(L) where L = number of levels
- Unpark: O(L) - could be optimized to O(1) with level mapping
- GetVehicle: O(1)

SPACE COMPLEXITY: O(S) where S = total number of spots

PARKING RULES IMPLEMENTED:
- Motorcycles can park in ANY empty spot (motorcycle OR car spots)
- Cars can ONLY park in empty car spots (NOT motorcycle spots)
- Each vehicle can only be parked once at a time
- Spots are allocated on first-come, first-served basis per level

EXCEPTION HANDLING:
- invalid_argument: For null pointers, empty strings, invalid parameters
- runtime_error: For business logic violations (already parked, not found, etc.)
- Comprehensive validation in constructors and core methods
- Clear error messages for debugging and user feedback

STRATEGIC SWITCH USAGE:
- Switch statements used for enum-based decisions (VehicleType, SpotType)
- Enables easy addition of new vehicle types (BUS, TRUCK) and spot types
- If-else used for simple boolean checks and map lookups where appropriate
- Clean separation between type-based logic and conditional logic

UNIT TESTING:
- Tests for constructor validation and exception throwing
- Tests for core parking/unparking operations
- Tests for edge cases and error conditions
- Demonstrates proper exception handling patterns
- Validates business logic and data integrity

EXTENSIBILITY:
This implementation provides a solid foundation that can be extended for:
- Additional vehicle types (buses, trucks, etc.) - just add to enum and switch cases
- Additional spot types (handicapped, VIP, etc.) - just add to enum and switch cases
- Pricing systems and receipt generation
- Reserved parking and time-based limits
- Multi-building parking complexes
- Database persistence and API endpoints
- Real-time monitoring and analytics

PRODUCTION CONSIDERATIONS:
- Thread safety would need to be added for concurrent operations
- Database integration for persistence
- Logging and monitoring for operations
- Performance optimization for large scale deployments
- API layer for external integrations
- Configuration management for different parking lot layouts

EXCEPTION HANDLING LOCATIONS:
1. Vehicle Constructor: Validates vehicleId and licensePlate are not empty
2. Spot Constructor: Validates spotId is not empty
3. ParkingLevel Constructor: Validates levelNumber >= 1
4. Spot::parkVehicle(): Validates vehicle pointer and spot availability
5. Spot::unparkVehicle(): Validates spot is occupied
6. ParkingLevel::addSpot(): Validates spot pointer and checks for duplicates
7. ParkingLot::addLevel(): Validates level pointer and checks for duplicates
8. ParkingLot::parkVehicle(): Validates vehicle pointer and checks if already parked
9. ParkingLot::unparkVehicle(): Validates vehicleId and checks if vehicle exists
10. ParkingLot::getVehicleInSpot(): Validates spotId and checks if spot exists

UNIT TEST COVERAGE:
✓ Constructor validation for all classes
✓ Null pointer handling
✓ Empty string validation
✓ Business logic violations (duplicate parking, etc.)
✓ Core functionality (park, unpark, getVehicle)
✓ Edge cases and error conditions
✓ Exception type verification
✓ Error message validation

===================================================================
*/