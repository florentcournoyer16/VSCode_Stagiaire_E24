.. _ExternalTelemetryUnit:

BLE and UWB: External telemetry unit
#########################

.. contents::
   :local:
   :depth: 2

The external telemetry unit sample demonstrates how to use the :ref:`ETU_readme`.

Requirements
************

The sample supports the following development kits:

.. table-from-sample-yaml::

.. include:: /includes/tfm.txt

The sample also requires a smartphone or tablet running a compatible application.
The `Testing`_ instructions refer to `nRF Connect for Mobile`_, but you can also use other similar applications (for example, `nRF Blinky`_ or `nRF Toolbox`_).

.. note::
   |thingy53_sample_note|

Overview
********

When connected, the sample sends the data received by UWB on the custom UdeS GRAMS board to the connected device, such as a phone or tablet.
The mobile application on the device can display the received data.

User interface
**************

The user interface of the sample depends on the hardware platform you are using.

nRF52840 Custom board
=====================

LED 0:
   Blinks, toggling on/off every second, when the main loop is running and the device is advertising.

LED 1:
   Lit when the development kit is connected.

Testing (Need to change description based on procedure!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!)
=======

After programming the sample to your dongle or development kit, test it by performing the following steps:

1. Start the `nRF Connect for Mobile`_ application on your smartphone or tablet.
#. Power on the development kit or insert your dongle into the USB port.
#. Connect to the device from the application.
   The device is advertising as ``Nordic_LBS``.
   The services of the connected device are shown.
#. In **Nordic LED Button Service**, enable notifications for the **Button** characteristic.
#. Press **Button 1** on the device.
#. Observe that notifications with the following values are displayed:

   * ``Button released`` when **Button 1** is released.
   * ``Button pressed`` when **Button 1** is pressed.

#. Write the following values to the LED characteristic in the **Nordic LED Button Service**.
   Depending on the hardware platform, this produces results described in the table.

+------------------------+---------+----------------------------------------------+
| Hardware platform      | Value   | Effect                                       |
+========================+=========+==============================================+
| Development kit        | ``OFF`` | Switch the **LED3** off.                     |
+                        +---------+----------------------------------------------+
|                        | ``ON``  | Switch the **LED3** on.                      |
+------------------------+---------+----------------------------------------------+
| nRF52840 Dongle        | ``OFF`` | Switch the green channel of the RGB LED off. |
+                        +---------+----------------------------------------------+
|                        | ``ON``  | Switch the green channel of the RGB LED on.  |
+------------------------+---------+----------------------------------------------+
| Thingy:53              | ``OFF`` | Switch the blue channel of the RGB LED off.  |
+                        +---------+----------------------------------------------+
|                        | ``ON``  | Switch the blue channel of the RGB LED on.   |
+------------------------+---------+----------------------------------------------+

Dependencies (Need to change dependencies based on project !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!)
************

This sample uses the following |NCS| libraries:

* :ref:`lbs_readme`
* :ref:`dk_buttons_and_leds_readme`

In addition, it uses the following Zephyr libraries:

* :file:`include/zephyr/types.h`
* :file:`lib/libc/minimal/include/errno.h`
* :file:`include/sys/printk.h`
* :file:`include/sys/byteorder.h`
* :ref:`GPIO Interface <zephyr:api_peripherals>`
* :ref:`zephyr:bluetooth_api`:

  * :file:`include/bluetooth/bluetooth.h`
  * :file:`include/bluetooth/hci.h`
  * :file:`include/bluetooth/conn.h`
  * :file:`include/bluetooth/uuid.h`
  * :file:`include/bluetooth/gatt.h`

The sample also uses the following secure firmware component:

* :ref:`Trusted Firmware-M <ug_tfm>`
