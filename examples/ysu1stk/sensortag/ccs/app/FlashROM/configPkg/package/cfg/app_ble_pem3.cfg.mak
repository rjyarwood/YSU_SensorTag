# invoke SourceDir generated makefile for app_ble.pem3
app_ble.pem3: .libraries,app_ble.pem3
.libraries,app_ble.pem3: package/cfg/app_ble_pem3.xdl
	$(MAKE) -f C:\ti\simplelink\ble_sdk_2_02_01_18\examples\ysu1stk\sensortag\ccs\config/src/makefile.libs

clean::
	$(MAKE) -f C:\ti\simplelink\ble_sdk_2_02_01_18\examples\ysu1stk\sensortag\ccs\config/src/makefile.libs clean

