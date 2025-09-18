/**
 * Simple Parking Lot System - Interview Version
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

class ParkingLot {
private:
    vector<Spot*> spots;
    unordered_map<string, string> vehicleToSpot;  // vehicleId -> spotId
    unordered_map<string, Spot*> spotMap;         // spotId -> Spot*
    
public:
    void addSpot(Spot* spot) {
        spots.push_back(spot);
        spotMap[spot->spotId] = spot;
    }
    
    // CORE FUNCTION 1: Park a vehicle
    bool parkVehicle(Vehicle* vehicle) {
        if (vehicleToSpot.count(vehicle->vehicleId)) {
            return false;  // Already parked
        }
        
        // Find first available spot
        for (Spot* spot : spots) {
            if (spot->canFitVehicle(vehicle->type)) {
                spot->parkVehicle(vehicle);
                vehicleToSpot[vehicle->vehicleId] = spot->spotId;
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
        int available = 0;
        for (Spot* spot : spots) {
            if (!spot->isOccupied) available++;
        }
        cout << "Parking Status: " << available << "/" << spots.size() 
             << " spots available" << endl;
    }
};

// Demo
int main() {
    ParkingLot parkingLot;
    
    // Create spots
    parkingLot.addSpot(new Spot("M1", SpotType::MOTORCYCLE));
    parkingLot.addSpot(new Spot("M2", SpotType::MOTORCYCLE));
    parkingLot.addSpot(new Spot("C1", SpotType::CAR));
    parkingLot.addSpot(new Spot("C2", SpotType::CAR));
    
    // Create vehicles
    Vehicle* motorcycle = new Vehicle("BIKE001", VehicleType::MOTORCYCLE, "ABC123");
    Vehicle* car = new Vehicle("CAR001", VehicleType::CAR, "XYZ789");
    
    // Test core functions
    cout << "=== Testing Core Functions ===" << endl;
    
    // 1. Park vehicles
    cout << "Park motorcycle: " << (parkingLot.parkVehicle(motorcycle) ? "Success" : "Failed") << endl;
    cout << "Park car: " << (parkingLot.parkVehicle(car) ? "Success" : "Failed") << endl;
    
    parkingLot.printStatus();
    
    // 3. Find vehicle in spot
    Vehicle* found = parkingLot.getVehicleInSpot("C1");
    cout << "Vehicle in C1: " << (found ? found->vehicleId : "Empty") << endl;
    
    // 2. Unpark vehicle
    cout << "Unpark car: " << (parkingLot.unparkVehicle("CAR001") ? "Success" : "Failed") << endl;
    
    parkingLot.printStatus();
    
    return 0;
}

/*
INTERVIEW TALKING POINTS:

PROBLEM: Simple parking lot with 3 core functions:
- Park vehicle, Unpark vehicle, Find vehicle in spot
- Rules: Motorcycles can park anywhere, Cars need car spots

SOLUTION APPROACH:
- Vehicle, Spot, ParkingLot classes
- Hash maps for O(1) lookups
- Linear search for available spots

TIME COMPLEXITY:
- Park: O(n) where n = number of spots
- Unpark: O(1)
- GetVehicle: O(1)

SPACE COMPLEXITY: O(n) for storing spots and mappings

FOLLOW-UP EXTENSIONS:
1. Buses need 5 consecutive spots - modify search logic
2. Add pricing - timestamp in Vehicle, calculate fees
3. Reserved spots - add reservation system
4. Size-based parking - add vehicle/spot sizes
5. Database persistence - replace in-memory storage
*/