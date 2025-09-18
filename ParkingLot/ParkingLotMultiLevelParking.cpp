/**
 * Parking Lot Management System - Interview Version
 * 
 * Core Requirements:
 * 1. Park a vehicle
 * 2. Unpark a vehicle  
 * 3. Find vehicle in a spot
 * 
 * Rules: Motorcycles can park anywhere, Cars need car spots only
 */

#include <bits/stdc++.h>
using namespace std;

enum class VehicleType { MOTORCYCLE, CAR };
enum class SpotType { MOTORCYCLE, CAR };

class Vehicle {
public:
    string vehicleId;
    VehicleType type;
    string licensePlate;
    
    Vehicle(const string& id, VehicleType t, const string& plate) 
        : vehicleId(id), type(t), licensePlate(plate) {}
};

class Spot {
public:
    string spotId;
    SpotType type;
    bool isOccupied;
    Vehicle* vehicle;
    
    Spot(const string& id, SpotType t) 
        : spotId(id), type(t), isOccupied(false), vehicle(nullptr) {}
    
    bool canFitVehicle(VehicleType vehicleType) {
        if (isOccupied) return false;
        
        if (vehicleType == VehicleType::MOTORCYCLE) return true;  // Can park anywhere
        return (type == SpotType::CAR);  // Cars need car spots only
    }
    
    void parkVehicle(Vehicle* v) {
        vehicle = v;
        isOccupied = true;
    }
    
    void unparkVehicle() {
        vehicle = nullptr;
        isOccupied = false;
    }
};

class ParkingLevel {
public:
    int levelNumber;
    vector<Spot*> spots;
    
    ParkingLevel(int level) : levelNumber(level) {}
    
    void addSpot(Spot* spot) {
        spots.push_back(spot);
    }
    
    string findAvailableSpot(VehicleType vehicleType) {
        for (Spot* spot : spots) {
            if (spot->canFitVehicle(vehicleType)) {
                return spot->spotId;
            }
        }
        return "";  // No spot found
    }
};

class ParkingLot {
private:
    vector<ParkingLevel*> levels;
    unordered_map<string, string> vehicleToSpot;  // vehicleId -> spotId
    unordered_map<string, Spot*> spotMap;         // spotId -> Spot*
    
public:
    void addLevel(ParkingLevel* level) {
        levels.push_back(level);
        // Register all spots for quick lookup
        for (Spot* spot : level->spots) {
            spotMap[spot->spotId] = spot;
        }
    }
    
    // CORE FUNCTION 1: Park a vehicle
    bool parkVehicle(Vehicle* vehicle) {
        if (vehicleToSpot.count(vehicle->vehicleId)) {
            return false;  // Already parked
        }
        
        // Find available spot across all levels
        for (ParkingLevel* level : levels) {
            string spotId = level->findAvailableSpot(vehicle->type);
            if (!spotId.empty()) {
                Spot* spot = spotMap[spotId];
                spot->parkVehicle(vehicle);
                vehicleToSpot[vehicle->vehicleId] = spotId;
                return true;
            }
        }
        return false;  // No space available
    }
    
    // CORE FUNCTION 2: Unpark a vehicle
    bool unparkVehicle(const string& vehicleId) {
        if (!vehicleToSpot.count(vehicleId)) {
            return false;  // Vehicle not found
        }
        
        string spotId = vehicleToSpot[vehicleId];
        Spot* spot = spotMap[spotId];
        spot->unparkVehicle();
        vehicleToSpot.erase(vehicleId);
        return true;
    }
    
    // CORE FUNCTION 3: Get vehicle in spot
    Vehicle* getVehicleInSpot(const string& spotId) {
        if (spotMap.count(spotId)) {
            return spotMap[spotId]->vehicle;
        }
        return nullptr;  // Spot not found
    }
    
    void printStatus() {
        cout << "\n=== Parking Status ===" << endl;
        for (ParkingLevel* level : levels) {
            int available = 0;
            for (Spot* spot : level->spots) {
                if (!spot->isOccupied) available++;
            }
            cout << "Level " << level->levelNumber << ": " << available 
                 << "/" << level->spots.size() << " spots available" << endl;
        }
    }
};

// Demo
int main() {
    ParkingLot parkingLot;
    
    // Create level with spots
    ParkingLevel* level1 = new ParkingLevel(1);
    level1->addSpot(new Spot("L1-M1", SpotType::MOTORCYCLE));
    level1->addSpot(new Spot("L1-C1", SpotType::CAR));
    level1->addSpot(new Spot("L1-C2", SpotType::CAR));
    
    parkingLot.addLevel(level1);
    
    // Create vehicles
    Vehicle* motorcycle = new Vehicle("M001", VehicleType::MOTORCYCLE, "BIKE123");
    Vehicle* car = new Vehicle("C001", VehicleType::CAR, "CAR456");
    
    // Test core functions
    cout << "=== Testing Core Functions ===" << endl;
    
    // 1. Park vehicles
    cout << "Park motorcycle: " << (parkingLot.parkVehicle(motorcycle) ? "Success" : "Failed") << endl;
    cout << "Park car: " << (parkingLot.parkVehicle(car) ? "Success" : "Failed") << endl;
    
    parkingLot.printStatus();
    
    // 3. Find vehicle in spot
    Vehicle* found = parkingLot.getVehicleInSpot("L1-C1");
    cout << "Vehicle in L1-C1: " << (found ? found->vehicleId : "Empty") << endl;
    
    // 2. Unpark vehicle
    cout << "Unpark car: " << (parkingLot.unparkVehicle("C001") ? "Success" : "Failed") << endl;
    
    parkingLot.printStatus();
    
    return 0;
}

/*
INTERVIEW TALKING POINTS:

PROBLEM: Design parking lot with 3 core functions:
- Park vehicle, Unpark vehicle, Find vehicle in spot
- Rules: Motorcycles can park anywhere, Cars need car spots

SOLUTION APPROACH:
- Vehicle, Spot, Level, ParkingLot classes
- Hash maps for O(1) lookups
- Simple parking rule logic

TIME COMPLEXITY:
- Park: O(L*S) where L=levels, S=spots per level  
- Unpark: O(1)
- GetVehicle: O(1)

FOLLOW-UP EXTENSIONS:
1. Buses need 5 consecutive spots - modify findAvailableSpot()
2. Add pricing - timestamp in Vehicle, calculate duration
3. Threading - add mutex locks around shared data
4. Database - replace hash maps with DB calls
5. Multiple vehicle sizes - add size field and spot capacity

KEY DESIGN DECISIONS:
- Used enums for type safety and extensibility
- Separated concerns: each class has single responsibility  
- Hash maps for fast lookups vs scanning arrays
- Simple boolean returns vs exceptions (interview preference)
*/