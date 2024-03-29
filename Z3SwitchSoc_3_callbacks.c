/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"

#include EMBER_AF_API_NETWORK_STEERING
#include EMBER_AF_API_ZLL_PROFILE
#include EMBER_AF_API_FIND_AND_BIND_INITIATOR

#define SWITCH_ENDPOINT (1)

static bool commissioning = false;

EmberEventControl commissioningEventControl;
EmberEventControl ledEventControl;
EmberEventControl findingAndBindingEventControl;
static uint8_t lastButton;

void commissioningEventHandler(void)
{
  EmberStatus status;

  emberEventControlSetInactive(commissioningEventControl);

  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    emberAfGetCommandApsFrame()->sourceEndpoint = SWITCH_ENDPOINT;
    if (lastButton == BUTTON0) {
    	emberAfCorePrintln("emberAfFillCommandOnOffClusterToggle");
      emberAfFillCommandOnOffClusterToggle();
    } else if (lastButton == BUTTON1) {
      uint8_t nextLevel = (uint8_t)(0xFF & emberGetPseudoRandomNumber());
      emberAfCorePrintln("emberAfFillCommandLevelControlClusterMoveToLevel");
      emberAfFillCommandLevelControlClusterMoveToLevel(nextLevel, TRANSITION_TIME_DS, 0, 0);
    }
    status = emberAfSendCommandUnicastToBindings();
    emberAfCorePrintln("%p: 0x%X", "Send to bindings", status);
  }
  else
  {
////    bool touchlink = (lastButton == BUTTON1);
//    status = (touchlink
//              ? emberAfZllInitiateTouchLink()
//              : emberAfPluginNetworkSteeringStart());
	  //    bool touchlink = (lastButton == BUTTON1);
	  status = emberAfPluginNetworkSteeringStart();

    emberAfCorePrintln("network steering return status : 0x%X", status);
//    emberEventControlSetActive(ledEventControl);
    commissioning = true;
  }
}

void ledEventHandler(void)
{
  emberEventControlSetInactive(ledEventControl);

  if (commissioning) {
    if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
    	emberAfCorePrintln("EMBER_JOINED_NETWORK");
//      halToggleLed(COMMISSIONING_STATUS_LED);
      emberEventControlSetDelayMS(ledEventControl, LED_BLINK_PERIOD_MS << 1);
    } else {
//      halSetLed(COMMISSIONING_STATUS_LED);
    }
  } else if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
//    halSetLed(COMMISSIONING_STATUS_LED);
  }
}

void findingAndBindingEventHandler(void)
{
  emberEventControlSetInactive(findingAndBindingEventControl);
  EmberStatus status = emberAfPluginFindAndBindInitiatorStart(SWITCH_ENDPOINT);
  emberAfCorePrintln("Find and bind initiator %p: 0x%X", "start", status);
}

static void scheduleFindingAndBindingForInitiator(void)
{
  emberEventControlSetDelayMS(findingAndBindingEventControl,
                              FINDING_AND_BINDING_DELAY_MS);
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
  if (status == EMBER_NETWORK_DOWN) {
//    halClearLed(COMMISSIONING_STATUS_LED);
  } else if (status == EMBER_NETWORK_UP) {
//    halSetLed(COMMISSIONING_STATUS_LED);
  }

  // This value is ignored by the framework.
  return false;
}

/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback(uint8_t button,
                                 uint8_t state)
{
  if (state == BUTTON_RELEASED) {
    lastButton = button;
    emberEventControlSetActive(commissioningEventControl);
  }
}



//HuyTV
void emberAfIdentifyClusterClientAttributeChangedCallback(uint8_t endpoint, EmberAfAttributeId attributeId)
{
	emberAfCorePrintln("emberAfIdentifyClusterClientAttributeChangedCallback endpoint %d, attributeId %d\r\n", endpoint, attributeId);
}

/** @brief On/off Cluster Client Attribute Changed
 *
 * Client Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */

// HuyTV
void emberAfOnOffClusterClientAttributeChangedCallback(int8u endpoint,
                                                       EmberAfAttributeId attributeId)
{
	emberAfCorePrintln("emberAfOnOffClusterClientAttributeChangedCallback endpoint %d, attributeId %d\r\n", endpoint, attributeId);
}


// HuyTV

void emberAfBasicClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                       EmberAfAttributeId attributeId)
{
	emberAfCorePrintln("emberAfBasicClusterServerAttributeChangedCallback endpoint %d, attributeId %d\r\n", endpoint, attributeId);
}

// HuyTV

static void vReportOnOffAttribute(uint8_t endpoint, bool is_on)
{
// The way 1
	 uint8_t data[] = { LOW_BYTE(ZCL_ON_OFF_ATTRIBUTE_ID), HIGH_BYTE(ZCL_ON_OFF_ATTRIBUTE_ID), ZCL_BOOLEAN_ATTRIBUTE_TYPE, 0};
	 data[3] = is_on;
	 emberAfFillCommandGlobalServerToClientReportAttributes( ZCL_ON_OFF_CLUSTER_ID, data, sizeof(data) );
	 emberAfSetCommandEndpoints (endpoint, 0);
	 emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);		// send to ZC

	 emberAfCorePrintln("vReportOnOffAttribute endpoint %d, value %d\r\n", endpoint, is_on);
}

// HuyTV
void emberAfOnOffClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                       EmberAfAttributeId attributeId)
{
	emberAfCorePrintln("emberAfBasicClusterServerAttributeChangedCallback endpoint %d, attributeId %d\r\n", endpoint, attributeId);

	vReportOnOffAttribute(endpoint, 1);
}


/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
 emberAfCorePrintln("emberAfPluginNetworkSteeringCompleteCallback, status %d", status);
  emberAfCorePrintln("%p network %p: 0x%X", "Join", "complete", status);
  if (status != EMBER_SUCCESS) {
    commissioning = false;
  } else {
    scheduleFindingAndBindingForInitiator();
  }
}

/** @brief Touch Link Complete
 *
 * This function is called by the ZLL Commissioning Common plugin when touch linking
 * completes.
 *
 * @param networkInfo The ZigBee and ZLL-specific information about the network
 * and target. Ver.: always
 * @param deviceInformationRecordCount The number of sub-device information
 * records for the target. Ver.: always
 * @param deviceInformationRecordList The list of sub-device information
 * records for the target. Ver.: always
 */
void emberAfPluginZllCommissioningCommonTouchLinkCompleteCallback(const EmberZllNetwork *networkInfo,
                                                                  uint8_t deviceInformationRecordCount,
                                                                  const EmberZllDeviceInfoRecord *deviceInformationRecordList)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Touchlink",
                     "complete",
                     EMBER_SUCCESS);

  scheduleFindingAndBindingForInitiator();
}

/** @brief Touch Link Failed
 *
 * This function is called by the ZLL Commissioning Client plugin if touch linking
 * fails.
 *
 * @param status The reason the touch link failed. Ver.: always
 */
void emberAfPluginZllCommissioningClientTouchLinkFailedCallback(EmberAfZllCommissioningStatus status)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Touchlink",
                     "complete",
                     EMBER_ERR_FATAL);

  commissioning = false;
}

/** @brief Complete
 *
 * This callback is fired by the initiator when the Find and Bind process is
 * complete.
 *
 * @param status Status code describing the completion of the find and bind
 * process Ver.: always
 */
void emberAfPluginFindAndBindInitiatorCompleteCallback(EmberStatus status)
{
  emberAfCorePrintln("Find and bind initiator %p: 0x%X", "complete", status);

  commissioning = false;
}
