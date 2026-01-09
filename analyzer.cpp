#include "analyzer.h"
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>

// Students may use ANY data structure internally
static std::unordered_map<std::string, long long> zoneCount;
static std::unordered_map<std::string, std::map<int, long long>> zoneHourCount;

void TripAnalyzer:: ingestFile(const std::string& csvPath) {
   // TODO:
   // - open file
   // - skip header
   // - skip malformed rows
   // - extract PickupZoneID and pickup hour
   // - aggregate counts  
	
   std::ifstream file(csvPath);
   if (!file.is_open()) return; //dosya açılmazsa bir şey yapma
   std::string line; //okunacak satırı alır
   if (!std::getline(file, line)) return; //headerı okur atlar
	
   while (std::getline(file, line)) {
       if (line.empty()) continue; //boş satırı atla
       int commaCount = 0;
       for (size_t i = 0; i < line.size(); i++) {
           if (line[i] == ',') commaCount++; // virgülleri say
       }
	   
       if (commaCount < 5) continue; //kolon sayısı 6dan azsa satırı geç
       //virgüllerin yerini bul
       int c1 = line.find(',');
	   int c2 = line.find(',', c1 + 1);
       int c3 = line.find(',', c2 + 1);
       int c4 = line.find(',', c3 + 1);
	   
       std::string pickupZone = line.substr(c1 + 1, c2 - c1 - 1);  //pickupZone bilgisini al
       if (pickupZone.empty()) continue; //pickupZone boşsa geç
	   
       std::string pickupDateTime = line.substr(c3 + 1, c4 - c3 - 1); //pickup tarih-saat bilgisini al
       //tarih ve zaman bilgileri doğru mu kontrol et
       if (pickupDateTime.size() < 13) continue;
       if (pickupDateTime[4] != '-' || pickupDateTime[7] != '-' || pickupDateTime[10] != ' ') continue;
       if (!isdigit(pickupDateTime[11]) || !isdigit(pickupDateTime[12])) continue;
	   
       int pickupHour = std::stoi(pickupDateTime.substr(11, 2)); //saati integera çevir
       if (pickupHour < 0 || pickupHour > 23) continue; //saat 0 23 arasında mı kontrol et
	   
       zoneCount[pickupZone]++; // toplam yolculuk sayısını artırır
       zoneHourCount[pickupZone][pickupHour]++; // o zone’daki belirli saat için yolculuk sayısını artırır
   }
}
bool compareZones(const ZoneCount& a, const ZoneCount& b) {
   if (a.count!=b.count) {
       return a.count > b.count; //trip sayısı eşit değilse fazla olan zoneu öne al
   }
   return a.zone < b.zone;//trip sayısı eşitse zoneu alfabetik sıraya göre karşılaştır
}
std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
   // TODO:
   // - sort by count desc, zone asc
   // - return first k
   std::vector<ZoneCount> zones;
   for (auto it= zoneCount.begin(); it!= zoneCount.end(); it++) { //mapi sıralama yapmak için vectore çevir
      //zone adı ve sayısını ata
       ZoneCount z;
       z.zone= it->first;
       z.count= it->second;
       zones.push_back(z); //vectore ekle
   }
   std::sort(zones.begin(), zones.end(), compareZones); //zoneları sırala
    //ilk k tanesini yazdır
   if ((int)zones.size()>k) {
       zones.resize(k);
   }
   return zones;
}
bool compareSlots(const SlotCount& a, const SlotCount& b) {
   if (a.count!=b.count) {
       return a.count > b.count; //trip sayısı eşit değilse fazla olan slotu öne al
   }
   if (a.zone != b.zone) {
       return a.zone< b.zone; //trip sayısı eşitse zoneu alfabetik olarak küçük olanı öne al
   }
   return a.hour< b.hour; //zone da aynıysa daha erken saat önce gelir
}
std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
   // TODO:
   // - sort by count desc, zone asc, hour asc
   // - return first k
   std::vector<SlotCount> slots; //mapi sıralama yapmak için vectore çevir
   for (auto it = zoneHourCount.begin(); it != zoneHourCount.end(); it++) { //her zoneu dolaş
       const std::string& zone = it->first; //zone adını al
       for (auto it2 =it->second.begin(); it2 != it->second.end(); it2++) {
           SlotCount s; //slot bilgisi oluştur
           //zone,saat ve sayıyı ata
           s.zone = zone;
           s.hour = it2->first;
           s.count = it2->second;
           slots.push_back(s); //slotu vektöre ekle
       }
   }
   std::sort(slots.begin(), slots.end(), compareSlots); //slotları sırala
    //ilk k tanesini yazdır
   if ((int)slots.size()>k) {
       slots.resize(k);
   }
   return slots;
}





