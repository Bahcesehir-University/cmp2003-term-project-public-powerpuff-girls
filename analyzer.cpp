#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <cctype>
 
// Students may use ANY data structure internally
static std::unordered_map<std::string, long long> zoneCount;
static std::unordered_map<std::string, std::map<int, long long>> zoneHourCount;
 
void TripAnalyzer::ingestFile(const std::string& csvPath) {
    // TODO:
    // - open file
    // - skip header
    // - skip malformed rows
    // - extract PickupZoneID and pickup hour
    // - aggregate counts
 
    std::ifstream file(csvPath);
    if (!file.is_open()) return;
 
    std::string line; 
    if (!std::getline(file, line)) return;
 
    while (std::getline(file, line)) {
        if (line.empty()) continue; 
        int commaCount = 0;
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] == ',') commaCount++;
        }
        if (commaCount < 5) continue;
 
        int c1 = line.find(',');
        int c2 = line.find(',', c1 + 1);
        int c3 = line.find(',', c2 + 1);
        int c4 = line.find(',', c3 + 1);
 
        std::string pickupZone = line.substr(c1 + 1, c2 - c1 - 1);
        if (pickupZone.empty()) continue;
 
        std::string pickupDateTime = line.substr(c3 + 1, c4 - c3 - 1);
 
        if (pickupDateTime.size() < 13) continue;
        if (pickupDateTime[4] != '-' || pickupDateTime[7] != '-' || pickupDateTime[10] != ' ') continue;
        if (!isdigit(pickupDateTime[11]) || !isdigit(pickupDateTime[12])) continue;
 
        int pickupHour = std::stoi(pickupDateTime.substr(11, 2));
        if (pickupHour < 0 || pickupHour > 23) continue;
 
        zoneCount[pickupZone]++;
        zoneHourCount[pickupZone][pickupHour]++;
    }
}
 
bool compareZones(const ZoneCount& a, const ZoneCount& b) {
    if (a.count!=b.count) {
        return a.count > b.count;
    }
    return a.zone < b.zone;
}
 
std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    // TODO:
    // - sort by count desc, zone asc
    // - return first k
 
    std::vector<ZoneCount> zones;
    for (auto it= zoneCount.begin(); it!= zoneCount.end(); it++) {
        ZoneCount z;
        z.zone= it->first;
        z.count= it->second;
        zones.push_back(z);
    }
 
    std::sort(zones.begin(), zones.end(), compareZones);
    if ((int)zones.size()>k) {
        zones.resize(k);
    }
    return zones;
}
 
bool compareSlots(const SlotCount& a, const SlotCount& b) {
    if (a.count!=b.count) {
        return a.count > b.count;
    }
    if (a.zone != b.zone) {
        return a.zone< b.zone;
    }
    return a.hour< b.hour;
}
 
std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    // TODO:
    // - sort by count desc, zone asc, hour asc
    // - return first k
    std::vector<SlotCount> slots;
    for (auto it = zoneHourCount.begin(); it != zoneHourCount.end(); it++) {
        const std::string& zone = it->first;
        for (auto it2 =it->second.begin(); it2 != it->second.end(); it2++) {
            SlotCount s;
            s.zone = zone;
            s.hour = it2->first;
            s.count = it2->second;
            slots.push_back(s);
        }
    }
    std::sort(slots.begin(), slots.end(), compareSlots);
    if ((int)slots.size()>k) {
        slots.resize(k);
    }
    return slots;
}

