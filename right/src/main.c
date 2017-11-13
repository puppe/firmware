#include "config.h"
#include "main.h"
#include "init_clock.h"
#include "init_peripherals.h"
#include "usb_composite_device.h"
#include "peripherals/led_driver.h"
#include "key_action.h"
#include "slave_scheduler.h"
#include "peripherals/test_led.h"
#include "usb_interfaces/usb_interface_basic_keyboard.h"
#include "usb_interfaces/usb_interface_media_keyboard.h"
#include "usb_protocol_handler.h"
#include "bus_pal_hardware.h"
#include "command.h"
#include "bootloader/wormhole.h"
#include "eeprom.h"
#include "right_key_matrix.h"
#include "key_scanner.h"
#include "key_states.h"
#include "usb_commands/usb_command_apply_config.h"
#include "peripherals/reset_button.h"

void updateUsbReports(void)
{
    if (!IsUsbBasicKeyboardReportSent || !IsUsbMediaKeyboardReportSent || /*!IsUsbSystemKeyboardReportSent ||*/ !IsUsbMouseReportSent) {
        return;
    }

    ResetActiveUsbBasicKeyboardReport();
    ResetActiveUsbMediaKeyboardReport();
    ResetActiveUsbSystemKeyboardReport();
    ResetActiveUsbMouseReport();

    UpdateActiveUsbReports();

    SwitchActiveUsbBasicKeyboardReport();
    SwitchActiveUsbMediaKeyboardReport();
    SwitchActiveUsbSystemKeyboardReport();
    SwitchActiveUsbMouseReport();

    IsUsbBasicKeyboardReportSent = false;
    IsUsbMediaKeyboardReportSent = false;
    IsUsbSystemKeyboardReportSent = false;
    IsUsbMouseReportSent = false;
}

bool IsEepromInitialized = false;
bool IsConfigInitialized = false;

void userConfigurationReadFinished(void)
{
    IsEepromInitialized = true;
}

void hardwareConfigurationReadFinished(void)
{
    EEPROM_LaunchTransfer(EepromOperation_Read, ConfigBufferId_StagingUserConfig, userConfigurationReadFinished);
}

void main(void)
{
    InitClock();
    InitPeripherals();

    if (!RESET_BUTTON_IS_PRESSED) {
        EEPROM_LaunchTransfer(EepromOperation_Read, ConfigBufferId_HardwareConfig, hardwareConfigurationReadFinished);
    }

    if (Wormhole.magicNumber == WORMHOLE_MAGIC_NUMBER && Wormhole.enumerationMode == EnumerationMode_BusPal) {
        Wormhole.magicNumber = 0;
        init_hardware();
        handleUsbBusPalCommand();
    } else {
        InitSlaveScheduler();
        KeyMatrix_Init(&RightKeyMatrix);
        InitKeyScanner();
        updateUsbReports();
        InitUsb();

        while (1) {
            if (!IsConfigInitialized && IsEepromInitialized) {
                UsbCommand_ApplyConfig();
                IsConfigInitialized = true;
            }
            updateUsbReports();
            __WFI();
        }
    }
}
