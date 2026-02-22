/*
 * Imsak and Duha Prayer Times - Multi-City Example
 * 
 * This example demonstrates Imsak (fasting start) and Duha (forenoon) times
 * across different cities with regional calculation preferences.
 * 
 * Imsak: Time to begin fasting (Fajr minus precautionary buffer/Temkin)
 * Duha: Forenoon prayer time (when sun reaches specific altitude)
 * 
 * Hardware: Any Arduino-compatible board
 * Output: Serial monitor at 9600 baud
 * 
 * Author: Adnan Saab
 * Version: 2.1.0
 */

#include <Arduino.h>
#include "PrayerTimes.h"

// Fixed test date for all cities
static const int DAY   = 1;
static const int MONTH = 3;
static const int YEAR  = 2026;

// Helper function to get month name
const char* getMonthName(int month) {
  const char* months[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
  };
  if (month >= 1 && month <= 12) {
    return months[month - 1];
  }
  return "Unknown";
}

// Helper function to format date
String formatDate(int day, int month, int year) {
  char buffer[32];
  sprintf(buffer, "%s %d, %d", getMonthName(month), day, year);
  return String(buffer);
}

void printCityWithImsakDuha(
  const char* label,
  float lat,
  float lon,
  int tzMinutes,
  const CalculationConfig& method,
  int imsakOffsetMinutes,
  float duhaAngle,
  const char* notes = nullptr
) {
  PrayerTimes pt(lat, lon, tzMinutes);
  
  if (!pt.isInitialized()) {
    Serial.print("ERROR: Invalid coordinates for ");
    Serial.println(label);
    return;
  }
  
  // Apply high-latitude rule if needed
  if (pt.isHighLatitude()) {
    pt.setHighLatitudeRule(MIDDLE_OF_NIGHT);
  }
  
  // Set calculation method and Imsak/Duha parameters
  pt.setCalculationMethod(method);
  pt.setImsakOffset(imsakOffsetMinutes);
  pt.setDuhaAngle(duhaAngle);

  PrayerTimesResult result = pt.calculate(DAY, MONTH, YEAR);

  if (!result.valid) {
    Serial.print("ERROR: Calculation failed for ");
    Serial.println(label);
    return;
  }

  Serial.println();
  Serial.print("=== "); Serial.println(label);
  
  if (notes != nullptr) {
    Serial.print("    "); Serial.println(notes);
  }
  
  Serial.print("    Method: "); Serial.println(method.name);
  Serial.print("    Imsak Offset: "); Serial.print(imsakOffsetMinutes); Serial.println(" min before Fajr");
  Serial.print("    Duha Angle: "); Serial.print(duhaAngle); Serial.println("° above horizon");
  Serial.println();
  
  int hour, minute;
  
  // Imsak
  pt.minutesToTime(result.imsak, hour, minute);
  Serial.print("  Imsak   (Fast Start)  : ");
  Serial.println(pt.formatTime12(hour, minute));
  
  // Fajr
  pt.minutesToTime(result.fajr, hour, minute);
  Serial.print("  Fajr    (True Dawn)   : ");
  Serial.println(pt.formatTime12(hour, minute));
  
  // Sunrise
  pt.minutesToTime(result.sunrise, hour, minute);
  Serial.print("  Sunrise               : ");
  Serial.println(pt.formatTime12(hour, minute));
  
  // Duha
  pt.minutesToTime(result.duha, hour, minute);
  Serial.print("  Duha    (Forenoon)    : ");
  Serial.println(pt.formatTime12(hour, minute));
  
  // Dhuhr
  pt.minutesToTime(result.dhuhr, hour, minute);
  Serial.print("  Dhuhr                 : ");
  Serial.println(pt.formatTime12(hour, minute));
  
  // Maghrib
  pt.minutesToTime(result.maghrib, hour, minute);
  Serial.print("  Maghrib (Fast End)    : ");
  Serial.println(pt.formatTime12(hour, minute));
  
  if (pt.isHighLatitude()) {
    Serial.println();
    Serial.println("  * High-latitude adjustments applied");
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("=== PrayerTimes v2.1 - Imsak & Duha Multi-City Test ===");
  Serial.print("Date: ");
  Serial.println(formatDate(DAY, MONTH, YEAR));
  Serial.println();
  Serial.println("Imsak: Time to begin fasting (configurable buffer before Fajr)");
  Serial.println("Duha:  Forenoon prayer time (sun elevation angle based)");
  Serial.println("============================================================");

  // Malaysia (JAKIM) - Uses 18-20 min Temkin, 4.7° Duha
  printCityWithImsakDuha(
    "Kuala Lumpur, Malaysia",
    3.1390, 101.6869, 480,
    CalculationMethods::JAKIM,
    18,      // 18 minutes Temkin
    4.7,     // 4°42' as used by JAKIM
    "Islamic authority: JAKIM"
  );

  // Turkey (Diyanet) - Uses 10-12 min Temkin, 4.5° Duha
  printCityWithImsakDuha(
    "Istanbul, Turkey",
    41.0082, 28.9784, 180,
    CalculationMethods::TURKEY,
    12,      // 12 minutes Temkin
    4.5,
    "Islamic authority: Diyanet"
  );

  // Indonesia - Uses 10 min, 3.5° Duha
  printCityWithImsakDuha(
    "Jakarta, Indonesia",
    -6.2088, 106.8456, 420,
    CalculationMethods::INDONESIA,
    10,
    3.5,
    "Regional: 3.5° Duha angle"
  );

  // Saudi Arabia (Umm al-Qura) - Default 10 min, 5.0° Duha
  printCityWithImsakDuha(
    "Makkah, Saudi Arabia",
    21.4225, 39.8262, 180,
    CalculationMethods::MAKKAH,
    10,
    5.0,
    "Umm al-Qura method"
  );

  // Egypt - Uses 10 min, 4.5° Duha
  printCityWithImsakDuha(
    "Cairo, Egypt",
    30.0444, 31.2357, 120,
    CalculationMethods::EGYPT,
    10,
    4.5,
    "Egyptian General Authority"
  );

  Serial.println();
  Serial.println("============================================================");
  Serial.println("Notes:");
  Serial.println("- Imsak offset is configurable (default: 10 minutes)");
  Serial.println("- Duha angle varies by region (3.5° - 5.0° typical range)");
  Serial.println("- Use setImsakOffset(minutes) and setDuhaAngle(degrees)");
  Serial.println("============================================================");
  Serial.println();
  Serial.println("=== End of Test ===");
}

void loop() {}
