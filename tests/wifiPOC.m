#include <stdio.h>

#import <CoreWLAN/CoreWLAN.h>
#import <Foundation/Foundation.h>

@interface CWNetworkProfile ()
@end

/**
 * Do i understand what i'm doing ? Absolutely not
 */

int main() {
  NSArray<CWInterface *> *interfaces = CWWiFiClient.sharedWiFiClient.interfaces;
  for (CWInterface *inf in interfaces) {
    printf("Ssid : %s\n",
           inf.configuration.networkProfiles.firstObject.ssid.UTF8String);
  }
  return 0;
}