#include <windows.h>
#include <d3d11.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dwrite.lib")
#include "dwrite.h"
#include "stdio.h"

IDWriteFactory* g_pDWriteFactory = nullptr;
IDWriteTextFormat* g_pTextFormat = nullptr;



ID2D1Factory* g_pD2DFactory = nullptr;
ID2D1RenderTarget* g_pD2DRenderTarget = nullptr;
ID2D1SolidColorBrush* g_pBrush = nullptr;

static float gpuTemps[100] = { 0 };       // GPU 温度数据
static float gpuHotspotTemps[100] = { 0 }; // GPU Hotspot 温度数据
static float gpuMemoryTemps[100] = { 0 };  // GPU Memory 温度数据

static int tempIndex = 0;  // 当前的温度数据索引


typedef enum _NvAPI_Status
{
    NVAPI_OK = 0,      //!< Success. Request is completed.
    NVAPI_ERROR = -1,      //!< Generic error
    NVAPI_LIBRARY_NOT_FOUND = -2,      //!< NVAPI support library cannot be loaded.
    NVAPI_NO_IMPLEMENTATION = -3,      //!< not implemented in current driver installation
    NVAPI_API_NOT_INITIALIZED = -4,      //!< NvAPI_Initialize has not been called (successfully)
    NVAPI_INVALID_ARGUMENT = -5,      //!< The argument/parameter value is not valid or NULL.
    NVAPI_NVIDIA_DEVICE_NOT_FOUND = -6,      //!< No NVIDIA display driver, or NVIDIA GPU driving a display, was found.
    NVAPI_END_ENUMERATION = -7,      //!< No more items to enumerate
    NVAPI_INVALID_HANDLE = -8,      //!< Invalid handle
    NVAPI_INCOMPATIBLE_STRUCT_VERSION = -9,      //!< An argument's structure version is not supported
    NVAPI_HANDLE_INVALIDATED = -10,     //!< The handle is no longer valid (likely due to GPU or display re-configuration)
    NVAPI_OPENGL_CONTEXT_NOT_CURRENT = -11,     //!< No NVIDIA OpenGL context is current (but needs to be)
    NVAPI_INVALID_POINTER = -14,     //!< An invalid pointer, usually NULL, was passed as a parameter
    NVAPI_NO_GL_EXPERT = -12,     //!< OpenGL Expert is not supported by the current drivers
    NVAPI_INSTRUMENTATION_DISABLED = -13,     //!< OpenGL Expert is supported, but driver instrumentation is currently disabled
    NVAPI_NO_GL_NSIGHT = -15,     //!< OpenGL does not support Nsight

    NVAPI_EXPECTED_LOGICAL_GPU_HANDLE = -100,    //!< Expected a logical GPU handle for one or more parameters
    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE = -101,    //!< Expected a physical GPU handle for one or more parameters
    NVAPI_EXPECTED_DISPLAY_HANDLE = -102,    //!< Expected an NV display handle for one or more parameters
    NVAPI_INVALID_COMBINATION = -103,    //!< The combination of parameters is not valid. 
    NVAPI_NOT_SUPPORTED = -104,    //!< Requested feature is not supported in the selected GPU
    NVAPI_PORTID_NOT_FOUND = -105,    //!< No port ID was found for the I2C transaction
    NVAPI_EXPECTED_UNATTACHED_DISPLAY_HANDLE = -106,    //!< Expected an unattached display handle as one of the input parameters.
    NVAPI_INVALID_PERF_LEVEL = -107,    //!< Invalid perf level 
    NVAPI_DEVICE_BUSY = -108,    //!< Device is busy; request not fulfilled
    NVAPI_NV_PERSIST_FILE_NOT_FOUND = -109,    //!< NV persist file is not found
    NVAPI_PERSIST_DATA_NOT_FOUND = -110,    //!< NV persist data is not found
    NVAPI_EXPECTED_TV_DISPLAY = -111,    //!< Expected a TV output display
    NVAPI_EXPECTED_TV_DISPLAY_ON_DCONNECTOR = -112,    //!< Expected a TV output on the D Connector - HDTV_EIAJ4120.
    NVAPI_NO_ACTIVE_SLI_TOPOLOGY = -113,    //!< SLI is not active on this device.
    NVAPI_SLI_RENDERING_MODE_NOTALLOWED = -114,    //!< Setup of SLI rendering mode is not possible right now.
    NVAPI_EXPECTED_DIGITAL_FLAT_PANEL = -115,    //!< Expected a digital flat panel.
    NVAPI_ARGUMENT_EXCEED_MAX_SIZE = -116,    //!< Argument exceeds the expected size.
    NVAPI_DEVICE_SWITCHING_NOT_ALLOWED = -117,    //!< Inhibit is ON due to one of the flags in NV_GPU_DISPLAY_CHANGE_INHIBIT or SLI active.
    NVAPI_TESTING_CLOCKS_NOT_SUPPORTED = -118,    //!< Testing of clocks is not supported.
    NVAPI_UNKNOWN_UNDERSCAN_CONFIG = -119,    //!< The specified underscan config is from an unknown source (e.g. INF)
    NVAPI_TIMEOUT_RECONFIGURING_GPU_TOPO = -120,    //!< Timeout while reconfiguring GPUs
    NVAPI_DATA_NOT_FOUND = -121,    //!< Requested data was not found
    NVAPI_EXPECTED_ANALOG_DISPLAY = -122,    //!< Expected an analog display
    NVAPI_NO_VIDLINK = -123,    //!< No SLI video bridge is present
    NVAPI_REQUIRES_REBOOT = -124,    //!< NVAPI requires a reboot for the settings to take effect
    NVAPI_INVALID_HYBRID_MODE = -125,    //!< The function is not supported with the current Hybrid mode.
    NVAPI_MIXED_TARGET_TYPES = -126,    //!< The target types are not all the same
    NVAPI_SYSWOW64_NOT_SUPPORTED = -127,    //!< The function is not supported from 32-bit on a 64-bit system.
    NVAPI_IMPLICIT_SET_GPU_TOPOLOGY_CHANGE_NOT_ALLOWED = -128,    //!< There is no implicit GPU topology active. Use NVAPI_SetHybridMode to change topology.
    NVAPI_REQUEST_USER_TO_CLOSE_NON_MIGRATABLE_APPS = -129,      //!< Prompt the user to close all non-migratable applications.    
    NVAPI_OUT_OF_MEMORY = -130,    //!< Could not allocate sufficient memory to complete the call.
    NVAPI_WAS_STILL_DRAWING = -131,    //!< The previous operation that is transferring information to or from this surface is incomplete.
    NVAPI_FILE_NOT_FOUND = -132,    //!< The file was not found.
    NVAPI_TOO_MANY_UNIQUE_STATE_OBJECTS = -133,    //!< There are too many unique instances of a particular type of state object.
    NVAPI_INVALID_CALL = -134,    //!< The method call is invalid. For example, a method's parameter may not be a valid pointer.
    NVAPI_D3D10_1_LIBRARY_NOT_FOUND = -135,    //!< d3d10_1.dll cannot be loaded.
    NVAPI_FUNCTION_NOT_FOUND = -136,    //!< Couldn't find the function in the loaded DLL.
    NVAPI_INVALID_USER_PRIVILEGE = -137,    //!< Current User is not Admin.
    NVAPI_EXPECTED_NON_PRIMARY_DISPLAY_HANDLE = -138,    //!< The handle corresponds to GDIPrimary.
    NVAPI_EXPECTED_COMPUTE_GPU_HANDLE = -139,    //!< Setting Physx GPU requires that the GPU is compute-capable.
    NVAPI_STEREO_NOT_INITIALIZED = -140,    //!< The Stereo part of NVAPI failed to initialize completely. Check if the stereo driver is installed.
    NVAPI_STEREO_REGISTRY_ACCESS_FAILED = -141,    //!< Access to stereo-related registry keys or values has failed.
    NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED = -142, //!< The given registry profile type is not supported.
    NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED = -143,    //!< The given registry value is not supported.
    NVAPI_STEREO_NOT_ENABLED = -144,    //!< Stereo is not enabled and the function needed it to execute completely.
    NVAPI_STEREO_NOT_TURNED_ON = -145,    //!< Stereo is not turned on and the function needed it to execute completely.
    NVAPI_STEREO_INVALID_DEVICE_INTERFACE = -146,    //!< Invalid device interface.
    NVAPI_STEREO_PARAMETER_OUT_OF_RANGE = -147,    //!< Separation percentage or JPEG image capture quality is out of [0-100] range.
    NVAPI_STEREO_FRUSTUM_ADJUST_MODE_NOT_SUPPORTED = -148, //!< The given frustum adjust mode is not supported.
    NVAPI_TOPO_NOT_POSSIBLE = -149,    //!< The mosaic topology is not possible given the current state of the hardware.
    NVAPI_MODE_CHANGE_FAILED = -150,    //!< An attempt to do a display resolution mode change has failed.        
    NVAPI_D3D11_LIBRARY_NOT_FOUND = -151,    //!< d3d11.dll/d3d11_beta.dll cannot be loaded.
    NVAPI_INVALID_ADDRESS = -152,    //!< Address is outside of valid range.
    NVAPI_STRING_TOO_SMALL = -153,    //!< The pre-allocated string is too small to hold the result.
    NVAPI_MATCHING_DEVICE_NOT_FOUND = -154,    //!< The input does not match any of the available devices.
    NVAPI_DRIVER_RUNNING = -155,    //!< Driver is running.
    NVAPI_DRIVER_NOTRUNNING = -156,    //!< Driver is not running.
    NVAPI_ERROR_DRIVER_RELOAD_REQUIRED = -157,    //!< A driver reload is required to apply these settings.
    NVAPI_SET_NOT_ALLOWED = -158,    //!< Intended setting is not allowed.
    NVAPI_ADVANCED_DISPLAY_TOPOLOGY_REQUIRED = -159,    //!< Information can't be returned due to "advanced display topology".
    NVAPI_SETTING_NOT_FOUND = -160,    //!< Setting is not found.
    NVAPI_SETTING_SIZE_TOO_LARGE = -161,    //!< Setting size is too large.
    NVAPI_TOO_MANY_SETTINGS_IN_PROFILE = -162,    //!< There are too many settings for a profile. 
    NVAPI_PROFILE_NOT_FOUND = -163,    //!< Profile is not found.
    NVAPI_PROFILE_NAME_IN_USE = -164,    //!< Profile name is duplicated.
    NVAPI_PROFILE_NAME_EMPTY = -165,    //!< Profile name is empty.
    NVAPI_EXECUTABLE_NOT_FOUND = -166,    //!< Application not found in the Profile.
    NVAPI_EXECUTABLE_ALREADY_IN_USE = -167,    //!< Application already exists in the other profile.
    NVAPI_DATATYPE_MISMATCH = -168,    //!< Data Type mismatch 
    NVAPI_PROFILE_REMOVED = -169,    //!< The profile passed as parameter has been removed and is no longer valid.
    NVAPI_UNREGISTERED_RESOURCE = -170,    //!< An unregistered resource was passed as a parameter. 
    NVAPI_ID_OUT_OF_RANGE = -171,    //!< The DisplayId corresponds to a display which is not within the normal outputId range.
    NVAPI_DISPLAYCONFIG_VALIDATION_FAILED = -172,    //!< Display topology is not valid so the driver cannot do a mode set on this configuration.
    NVAPI_DPMST_CHANGED = -173,    //!< Display Port Multi-Stream topology has been changed.
    NVAPI_INSUFFICIENT_BUFFER = -174,    //!< Input buffer is insufficient to hold the contents.    
    NVAPI_ACCESS_DENIED = -175,    //!< No access to the caller.
    NVAPI_MOSAIC_NOT_ACTIVE = -176,    //!< The requested action cannot be performed without Mosaic being enabled.
    NVAPI_SHARE_RESOURCE_RELOCATED = -177,    //!< The surface is relocated away from video memory.
    NVAPI_REQUEST_USER_TO_DISABLE_DWM = -178,    //!< The user should disable DWM before calling NvAPI.
    NVAPI_D3D_DEVICE_LOST = -179,    //!< D3D device status is D3DERR_DEVICELOST or D3DERR_DEVICENOTRESET - the user has to reset the device.
    NVAPI_INVALID_CONFIGURATION = -180,    //!< The requested action cannot be performed in the current state.
    NVAPI_STEREO_HANDSHAKE_NOT_DONE = -181,    //!< Call failed as stereo handshake not completed.
    NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS = -182,    //!< The path provided was too short to determine the correct NVDRS_APPLICATION
    NVAPI_DEFAULT_STEREO_PROFILE_IS_NOT_DEFINED = -183,    //!< Default stereo profile is not currently defined
    NVAPI_DEFAULT_STEREO_PROFILE_DOES_NOT_EXIST = -184,    //!< Default stereo profile does not exist
    NVAPI_CLUSTER_ALREADY_EXISTS = -185,    //!< A cluster is already defined with the given configuration.
    NVAPI_DPMST_DISPLAY_ID_EXPECTED = -186,    //!< The input display id is not that of a multi stream enabled connector or a display device in a multi stream topology 
    NVAPI_INVALID_DISPLAY_ID = -187,    //!< The input display id is not valid or the monitor associated to it does not support the current operation
    NVAPI_STREAM_IS_OUT_OF_SYNC = -188,    //!< While playing secure audio stream, stream goes out of sync
    NVAPI_INCOMPATIBLE_AUDIO_DRIVER = -189,    //!< Older audio driver version than required
    NVAPI_VALUE_ALREADY_SET = -190,    //!< Value already set, setting again not allowed.
    NVAPI_TIMEOUT = -191,    //!< Requested operation timed out 
    NVAPI_GPU_WORKSTATION_FEATURE_INCOMPLETE = -192,    //!< The requested workstation feature set has incomplete driver internal allocation resources
    NVAPI_STEREO_INIT_ACTIVATION_NOT_DONE = -193,    //!< Call failed because InitActivation was not called.
    NVAPI_SYNC_NOT_ACTIVE = -194,    //!< The requested action cannot be performed without Sync being enabled.
    NVAPI_SYNC_MASTER_NOT_FOUND = -195,    //!< The requested action cannot be performed without Sync Master being enabled.
    NVAPI_INVALID_SYNC_TOPOLOGY = -196,    //!< Invalid displays passed in the NV_GSYNC_DISPLAY pointer.
    NVAPI_ECID_SIGN_ALGO_UNSUPPORTED = -197,    //!< The specified signing algorithm is not supported. Either an incorrect value was entered or the current installed driver/hardware does not support the input value.
    NVAPI_ECID_KEY_VERIFICATION_FAILED = -198,    //!< The encrypted public key verification has failed.
} NvAPI_Status;
HMODULE hmodNvapi = NULL;
#define NVAPI_MAX_PHYSICAL_GPUS             64
#define NVAPI_INTERFACE extern __success(return == NVAPI_OK) NvAPI_Status __cdecl
typedef enum
{
    NVAPI_THERMAL_TARGET_NONE = 0,
    NVAPI_THERMAL_TARGET_GPU = 1,     //!< GPU core temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_MEMORY = 2,     //!< GPU memory temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_POWER_SUPPLY = 4,     //!< GPU power supply temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_BOARD = 8,     //!< GPU board ambient temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_VCD_BOARD = 9,     //!< Visual Computing Device Board temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_INLET = 10,    //!< Visual Computing Device Inlet temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_OUTLET = 11,    //!< Visual Computing Device Outlet temperature requires NvVisualComputingDeviceHandle

    NVAPI_THERMAL_TARGET_ALL = 15,
    NVAPI_THERMAL_TARGET_UNKNOWN = -1,
} NV_THERMAL_TARGET;
typedef enum
{
    NVAPI_THERMAL_CONTROLLER_NONE = 0,
    NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL,
    NVAPI_THERMAL_CONTROLLER_ADM1032,
    NVAPI_THERMAL_CONTROLLER_MAX6649,
    NVAPI_THERMAL_CONTROLLER_MAX1617,
    NVAPI_THERMAL_CONTROLLER_LM99,
    NVAPI_THERMAL_CONTROLLER_LM89,
    NVAPI_THERMAL_CONTROLLER_LM64,
    NVAPI_THERMAL_CONTROLLER_ADT7473,
    NVAPI_THERMAL_CONTROLLER_SBMAX6649,
    NVAPI_THERMAL_CONTROLLER_VBIOSEVT,
    NVAPI_THERMAL_CONTROLLER_OS,
    NVAPI_THERMAL_CONTROLLER_UNKNOWN = -1,
} NV_THERMAL_CONTROLLER;
#define NVAPI_MAX_THERMAL_SENSORS_PER_GPU 3
typedef struct
{
    DWORD   version;                //!< structure version
    DWORD   count;                  //!< number of associated thermal sensors
    struct
    {
        NV_THERMAL_CONTROLLER       controller;         //!< internal, ADM1032, MAX6649...
        signed int                       defaultMinTemp;     //!< Minimum default temperature value of the thermal sensor in degree Celsius
        signed int                       defaultMaxTemp;     //!< Maximum default temperature value of the thermal sensor in degree Celsius
        signed int                       currentTemp;        //!< Current temperature value of the thermal sensor in degree Celsius
        NV_THERMAL_TARGET           target;             //!< Thermal sensor targeted - GPU, memory, chipset, powersupply, Visual Computing Device, etc
    } sensor[NVAPI_MAX_THERMAL_SENSORS_PER_GPU];

} NV_GPU_THERMAL_SETTINGS;


//total size 0xAB0

typedef struct _NV_API_TEMP_INDEX {
    DWORD Version;
    DWORD QueryTempIndex;
    BYTE Unknown[0xAA1];

    BYTE GpuHotspotTempIndex;
    BYTE Unknown2;
    BYTE MemoryTempIndex;
    BYTE Unknown3[4];
}NV_API_TEMP_INDEX, * PNV_API_TEMP_INDEX;

//total size 0xA8

typedef struct _NV_API_ADV_TEMPERATURE {
    DWORD Version;
    DWORD QueryTempIndex;

    BYTE Unknown[0x20];

    DWORD TempTable[32];

}NV_API_ADV_TEMPERATURE, * PNV_API_ADV_TEMPERATURE;


#define NVAPI_SHORT_STRING_MAX      64

typedef char NvAPI_ShortString[NVAPI_SHORT_STRING_MAX];

typedef PVOID(__fastcall* PNvAPI_QueryInterface)(DWORD Code);
typedef NvAPI_Status(__cdecl* PNvAPI_EnumPhysicalGPUs)(HANDLE nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS], DWORD* pGpuCount);
typedef NvAPI_Status(__cdecl* PNvAPI_GPU_GetThermalSettings)(HANDLE hPhysicalGpu, DWORD sensorIndex, NV_GPU_THERMAL_SETTINGS* pThermalSettings);

typedef NvAPI_Status(__cdecl* PNvAPI_GPU_GetFullName)(HANDLE hPhysicalGpu, NvAPI_ShortString szName);
typedef NvAPI_Status(__cdecl* PNvAPI_Initialize)();
typedef NvAPI_Status(__cdecl* PNvAPI_GetErrorMessage)(NvAPI_Status nr, NvAPI_ShortString szDesc);
typedef NvAPI_Status(__cdecl* PNvAPI_GPU_GetTempIndex)(HANDLE hPhysicalGpu, NV_API_TEMP_INDEX* TempIndex);
typedef NvAPI_Status(__cdecl* PNvAPI_GPU_GetTemperatureEx)(HANDLE hPhysicalGpu, NV_API_ADV_TEMPERATURE* AdvTemp);


PNvAPI_EnumPhysicalGPUs NvAPI_EnumPhysicalGPUs;
PNvAPI_GPU_GetThermalSettings NvAPI_GPU_GetThermalSettings;
PNvAPI_GPU_GetFullName NvAPI_GPU_GetFullName;
PNvAPI_Initialize NvAPI_Initialize;
PNvAPI_GetErrorMessage NvAPI_GetErrorMessage;
PNvAPI_GPU_GetTempIndex NvAPI_GPU_GetTempIndex;
PNvAPI_GPU_GetTemperatureEx NvAPI_GPU_GetTemperatureEx;

HANDLE PhyGPUHandles[NVAPI_MAX_PHYSICAL_GPUS];
DWORD GpuCount = 0;
NvAPI_ShortString GPUNames[NVAPI_MAX_PHYSICAL_GPUS];
NvAPI_ShortString ErrorMsg;
#define MAKE_NVAPI_VERSION(typeName,ver) (DWORD)(sizeof(typeName) | ((ver) << 16))


//! Macro for constructing the version field of NV_GPU_THERMAL_SETTINGS_V2
#define NV_GPU_THERMAL_SETTINGS_VER_2   MAKE_NVAPI_VERSION(NV_GPU_THERMAL_SETTINGS,2)

//! Macro for constructing the version field of NV_GPU_THERMAL_SETTINGS
#define NV_GPU_THERMAL_SETTINGS_VER     NV_GPU_THERMAL_SETTINGS_VER_2

#define NV_GPU_TEMP_INDEX_VER MAKE_NVAPI_VERSION(NV_API_TEMP_INDEX , 2)
#define NV_GPU_ADV_TEMP_VER MAKE_NVAPI_VERSION(NV_API_ADV_TEMPERATURE , 2)


VOID ReadyForGPU()
{
    //Only support NV GPU for now

    NV_GPU_THERMAL_SETTINGS thermalSettings = { NV_GPU_THERMAL_SETTINGS_VER };

    hmodNvapi = LoadLibraryA("nvapi64.dll");
    PNvAPI_QueryInterface NvAPI_QueryInterface;

    if (hmodNvapi == 0)
    {
        //printf("no nvapi %u\n", GetLastError());
        return;
    }

    NvAPI_QueryInterface = (PNvAPI_QueryInterface)GetProcAddress(hmodNvapi, "nvapi_QueryInterface");

    if (NvAPI_QueryInterface == NULL)
    {
        //printf("unable to location nvapi_QueryInterface\n");
        FreeLibrary(hmodNvapi);
        hmodNvapi = 0;
        return;
    }

    NvAPI_Initialize = (PNvAPI_Initialize)NvAPI_QueryInterface(0x150E828UL);
    NvAPI_GPU_GetFullName = (PNvAPI_GPU_GetFullName)NvAPI_QueryInterface(0xCEEE8E9FUL);
    NvAPI_GPU_GetThermalSettings = (PNvAPI_GPU_GetThermalSettings)NvAPI_QueryInterface(0xE3640A56UL);
    NvAPI_EnumPhysicalGPUs = (PNvAPI_EnumPhysicalGPUs)NvAPI_QueryInterface(0xE5AC921FUL);
    NvAPI_GetErrorMessage = (PNvAPI_GetErrorMessage)NvAPI_QueryInterface(0x6C2D048CUL);
    NvAPI_GPU_GetTempIndex = (PNvAPI_GPU_GetTempIndex)NvAPI_QueryInterface(0xBC8163D);
    NvAPI_GPU_GetTemperatureEx = (PNvAPI_GPU_GetTemperatureEx)NvAPI_QueryInterface(0x65FE3AAD);

    if (NvAPI_Initialize == 0 || NvAPI_GPU_GetFullName == 0 || NvAPI_GPU_GetThermalSettings == 0 || NvAPI_EnumPhysicalGPUs == 0 || NvAPI_GetErrorMessage == 0)
    {
        //printf("unable to init basic nvapis\n");
        FreeLibrary(hmodNvapi);
        hmodNvapi = 0;
        return;
    }

    if (NvAPI_GPU_GetTempIndex == 0 || NvAPI_GPU_GetTemperatureEx == 0)
    {
        //printf("unable to init undocumented temperature nvapis\n");
        FreeLibrary(hmodNvapi);
        hmodNvapi = 0;
        return;
    }


    NvAPI_Status Status = NvAPI_Initialize();

    if (Status != NVAPI_OK)
    {
        NvAPI_GetErrorMessage(Status, ErrorMsg);
        //printf("Failed to init NVAPI, Reason: %s\n", ErrorMsg);
        FreeLibrary(hmodNvapi);
        hmodNvapi = 0;
        return;
    }

    Status = NvAPI_EnumPhysicalGPUs(PhyGPUHandles, &GpuCount);

    if (Status != NVAPI_OK)
    {
        NvAPI_GetErrorMessage(Status, ErrorMsg);
        //printf("Failed to enum physical GPUs, Reason: %s\n", ErrorMsg);
        FreeLibrary(hmodNvapi);
        hmodNvapi = 0;
        return;
    }

    DWORD n;

    for (n = 0; n < GpuCount; n++)
    {
        Status = NvAPI_GPU_GetFullName(PhyGPUHandles[n], GPUNames[n]);

        if (Status != NVAPI_OK)
        {
            NvAPI_GetErrorMessage(Status, ErrorMsg);
            //printf("Failed to get GPU full name for GPU%u, Reason: %s\n", n, ErrorMsg);
            FreeLibrary(hmodNvapi);
            hmodNvapi = 0;
            return;
        }
    }




    return;
}

int coreTemp;
int hotspotTemp;
int memoryTemp;


CHAR GPUName[100];
VOID UpdateGPUTemp()
{
    DWORD n;
    NvAPI_Status Status;
    NV_GPU_THERMAL_SETTINGS thermalSettings = { NV_GPU_THERMAL_SETTINGS_VER };
    DWORD n2;
    NV_API_TEMP_INDEX TempIndex;
    NV_API_ADV_TEMPERATURE AdvTemp;


    //WCHAR ww[100];
    //swprintf_s(ww, 100, L"core:%u, hotspot:%u mem:%u\n", coreTemp, hotspotTemp, memoryTemp);
    //OutputDebugString(ww);

    if (hmodNvapi == 0)
    {
        return;
    }
    for (n = 0; n < GpuCount; n++)
    {
        Status = NvAPI_GPU_GetThermalSettings(PhyGPUHandles[n], NVAPI_THERMAL_TARGET_ALL, &thermalSettings);

        if (Status != NVAPI_OK)
        {
            NvAPI_GetErrorMessage(Status, ErrorMsg);
            //printf("Failed to get GPU thermal settings for GPU%u, Reason: %s\n", n, ErrorMsg);
            return;
        }

        //strcpy_s(Msg, "GPU: ");
        //strcat_s(Msg, GPUNames[n]);
        //strcat_s(Msg, " ");
        strcpy_s(GPUName, 100, GPUNames[n]);

        for (n2 = 0; n2 < thermalSettings.count; n2++)
        {
            NV_THERMAL_TARGET target = thermalSettings.sensor[n2].target;

            if (target == 1)
            {
                coreTemp = thermalSettings.sensor[n2].currentTemp;
                //  sprintf_s(Number, "GPU Core: %u°C  ", thermalSettings.sensor[n2].currentTemp);
            }
            else if (target == 2)
            {
                //  memoryTemp = thermalSettings.sensor[n2].currentTemp;
                 // sprintf_s(Number, "Memory: %u°C  ", thermalSettings.sensor[n2].currentTemp);
            }
            //else if (target == 4)
            //{
            //    sprintf_s(Number, "Power Supply: %u°C  ", thermalSettings.sensor[n2].currentTemp);
            //}
            //else if (target == 8)
            //{
            //    sprintf_s(Number, "Board: %u°C  ", thermalSettings.sensor[n2].currentTemp);
            //}
            //else
            //{
            //    sprintf_s(Number, "Unknown Part%u: %u°C  ", target, thermalSettings.sensor[n2].currentTemp);

            //}

          //  strcat_s(Msg, Number);
        }

        memset(&TempIndex, 0, sizeof(NV_API_TEMP_INDEX));
        memset(&AdvTemp, 0, sizeof(NV_API_ADV_TEMPERATURE));


        TempIndex.Version = NV_GPU_TEMP_INDEX_VER;

        Status = NvAPI_GPU_GetTempIndex(PhyGPUHandles[n], &TempIndex);

        if (Status != NVAPI_OK)
        {
            NvAPI_GetErrorMessage(Status, ErrorMsg);
            //   printf("Failed to get GPU temp index for GPU%u, Reason: %s\n", n, ErrorMsg);
        }
        else
        {
            if (TempIndex.GpuHotspotTempIndex >= 0x20 && TempIndex.MemoryTempIndex >= 0x20)
            {
                // printf("Index for GPU hotspot and memory both invalid for GPU %u\n", n);
            }
            else
            {

                AdvTemp.Version = NV_GPU_ADV_TEMP_VER;
                AdvTemp.QueryTempIndex = TempIndex.QueryTempIndex;

                Status = NvAPI_GPU_GetTemperatureEx(PhyGPUHandles[n], &AdvTemp);

                if (Status != NVAPI_OK)
                {
                    NvAPI_GetErrorMessage(Status, ErrorMsg);
                    // printf("Failed to get GPU advanced temperature for GPU%u, Reason: %s\n", n, ErrorMsg);
                }
                else
                {
                    if (TempIndex.GpuHotspotTempIndex < 0x20)
                    {
                        hotspotTemp = AdvTemp.TempTable[TempIndex.GpuHotspotTempIndex] / 256;
                        //sprintf_s(Number, "GPU Hotspot: %.0f°C  ", (float)AdvTemp.TempTable[TempIndex.GpuHotspotTempIndex] / 256);
                        //strcat_s(Msg, Number);
                    }

                    if (TempIndex.MemoryTempIndex < 0x20)
                    {
                        memoryTemp = AdvTemp.TempTable[TempIndex.MemoryTempIndex] / 256;
                        //sprintf_s(Number, "Memory: %.0f°C  ", (float)AdvTemp.TempTable[TempIndex.MemoryTempIndex] / 256);
                        //strcat_s(Msg, Number);
                    }
                }


            }


        }


        /*    strcat_s(Msg, "\n");

            printtobuffer(Msg);*/
    }
}



void UpdateTemperature()
{
    //// 模拟 GPU 温度变化
    //float currentGpuTemp = 50.0f + (rand() % 1000) / 100.0f;   // 50°C - 60°C
    //float currentGpuHotspotTemp = 60.0f + (rand() % 1000) / 100.0f; // 60°C - 70°C
    //float currentGpuMemoryTemp = 40.0f + (rand() % 1000) / 100.0f;  // 40°C - 50°C

    UpdateGPUTemp();

    // 保存温度数据
    gpuTemps[tempIndex] = (float)coreTemp;
    gpuHotspotTemps[tempIndex] = (float)hotspotTemp;
    gpuMemoryTemps[tempIndex] = (float)memoryTemp;


    // 更新索引
    tempIndex = (tempIndex + 1) % 100;
}void DrawTemperatureGraph()
{
    g_pD2DRenderTarget->BeginDraw();

    // 清除背景为完全透明
    g_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));  // 完全透明背景

    // 绘制一个半透明的灰色遮罩
    D2D1_RECT_F rect = D2D1::RectF(0.0f, 0.0f, 200.0f, 150.0f);
    g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Gray, 0.3f));  // 半透明灰色 (透明度 0.3)
    g_pD2DRenderTarget->FillRectangle(rect, g_pBrush);            // 绘制半透明背景

    // 定义窗口大小和温度范围
    const int windowWidth = 200;
    const int windowHeight = 150;
    const float minTemp = 0.0f;   // 最低温度
    const float maxTemp = 100.0f;  // 最高温度
    const float tempRange = maxTemp - minTemp;

    // 每个点在 x 轴上的间距
    float xInterval = (float)windowWidth / 100;  // 100 个数据点

    {
        // 绘制纵轴和刻度值
        const int axisX = 0;  // 纵轴距离窗口左边 10 像素
        const int numTicks = 4;  // 刻度数量 (25, 50, 75, 100)
        float tickStep = (float)windowHeight / (numTicks);  // 每个刻度的间隔
        float tempStep = tempRange / (numTicks);  // 每个刻度对应的温度值

        g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));  // 纵轴颜色为白色

        // 绘制纵轴线
        g_pD2DRenderTarget->DrawLine(
            D2D1::Point2F((FLOAT)axisX, 35.0f),
            D2D1::Point2F((FLOAT)axisX, (FLOAT)windowHeight),
            g_pBrush,
            2.0f
        );

        // 绘制刻度线和刻度值
        for (int i = 0; i < numTicks; ++i)
        {
            float yPos = windowHeight - i * tickStep;  // 从下往上绘制

            // 绘制刻度线
            g_pD2DRenderTarget->DrawLine(
                D2D1::Point2F(axisX, yPos),
                D2D1::Point2F(axisX + 8.0f, yPos),
                g_pBrush,
                1.0f
            );

            //// 计算对应的温度值
            //float tempValue = minTemp + i * tempStep;

            //// 将温度值转换为宽字符文本
            //wchar_t tempText[10];
            //swprintf_s(tempText, L"%.0f", tempValue);

            //// 绘制刻度值
            //D2D1_RECT_F textRect = D2D1::RectF(0.0f, yPos - 13.0f, axisX - 5.0f, yPos + 13.0f);

            //g_pD2DRenderTarget->DrawText(
            //    tempText,
            //    (UINT32)wcslen(tempText),
            //    g_pTextFormat,
            //    textRect,
            //    g_pBrush
            //);
        }

    }

    // 绘制 GPU 温度曲线 (红色)
    g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
    for (int i = 1; i < 100; ++i)
    {
        float x1 = (i - 1) * xInterval;
        float y1 = windowHeight - ((gpuTemps[(tempIndex + i - 1) % 100] - minTemp) / tempRange * windowHeight);
        float x2 = i * xInterval;
        float y2 = windowHeight - ((gpuTemps[(tempIndex + i) % 100] - minTemp) / tempRange * windowHeight);

        g_pD2DRenderTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), g_pBrush, 2.0f);
    }

    // 绘制 GPU Hotspot 温度曲线 (青色)
    g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Cyan));
    for (int i = 1; i < 100; ++i)
    {
        float x1 = (i - 1) * xInterval;
        float y1 = windowHeight - ((gpuHotspotTemps[(tempIndex + i - 1) % 100] - minTemp) / tempRange * windowHeight);
        float x2 = i * xInterval;
        float y2 = windowHeight - ((gpuHotspotTemps[(tempIndex + i) % 100] - minTemp) / tempRange * windowHeight);

        g_pD2DRenderTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), g_pBrush, 2.0f);
    }

    // 绘制 GPU Memory 温度曲线 (黄色)
    g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow));
    for (int i = 1; i < 100; ++i)
    {
        float x1 = (i - 1) * xInterval;
        float y1 = windowHeight - ((gpuMemoryTemps[(tempIndex + i - 1) % 100] - minTemp) / tempRange * windowHeight);
        float x2 = i * xInterval;
        float y2 = windowHeight - ((gpuMemoryTemps[(tempIndex + i) % 100] - minTemp) / tempRange * windowHeight);

        g_pD2DRenderTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), g_pBrush, 2.0f);
    }

    // 准备文本
    //wchar_t text[40];
    int wcharStrLen = MultiByteToWideChar(CP_ACP, 0, GPUName, -1, NULL, 0);
    //swprintf_s(text, 40 , L"%s", GPUName);
      // 创建存储转换后宽字符的缓冲区
    wchar_t* text = new wchar_t[wcharStrLen];

    // 执行转换
    MultiByteToWideChar(CP_ACP, 0, GPUName, -1, text, wcharStrLen);


    D2D1_RECT_F layoutRect = D2D1::RectF(10.0f, 10.0f, 200.0f, 30.0f);  // 文本显示区域

    // 设置文本颜色
    g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightGreen));

    // 绘制文本
    g_pD2DRenderTarget->DrawText(
        text,                    // 要绘制的文本
        (UINT32)wcslen(text),            // 文本长度
        g_pTextFormat,           // 文本格式
        layoutRect,              // 文本绘制区域
        g_pBrush                 // 文本颜色刷
    );
    delete[] text;

    g_pD2DRenderTarget->EndDraw();
}


// 全局变量
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

// 窗口过程函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 初始化 Direct3D
HRESULT InitD3D(HWND hWnd)
{
    // 描述交换链
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 200;
    sd.BufferDesc.Height = 150;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // 创建设备、交换链及设备上下文
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, nullptr, &g_pImmediateContext);

    if (FAILED(hr)) return hr;

    // 获取后缓冲区并创建渲染目标视图
    ID3D11Texture2D* pBackBuffer = nullptr;

    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (pBackBuffer == nullptr)
    {
        return S_FALSE;
    }

    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();

    // 设置渲染目标
    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    return S_OK;
}

// 清理 Direct3D 资源
void CleanupDevice()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}


HRESULT InitD2D(HWND hWnd)
{
    HRESULT hr = S_OK;

    // 创建 Direct2D 工厂
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    if (FAILED(hr)) return hr;

    // 获取交换链的后缓冲区
    IDXGISurface* pSurface = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pSurface);
    if (FAILED(hr)) return hr;

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
    );
    hr = g_pD2DFactory->CreateDxgiSurfaceRenderTarget(pSurface, &props, &g_pD2DRenderTarget);
    pSurface->Release();

    if (FAILED(hr)) return hr;

    // 创建画刷
    hr = g_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &g_pBrush);
    return hr;
}
// 清理 Direct2D 资源
void CleanupD2D()
{
    if (g_pBrush) g_pBrush->Release();
    if (g_pD2DRenderTarget) g_pD2DRenderTarget->Release();
    if (g_pD2DFactory) g_pD2DFactory->Release();
}
void Render()
{
    UpdateTemperature();



    // 清除背景色
    float ClearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

    // 绘制温度图表
    DrawTemperatureGraph();

    // 交换前后缓冲区
    g_pSwapChain->Present(0, 0);
    return;
}

DWORD WINAPI RenderThread(LPVOID lpThreadParameter)
{

    while (TRUE)
    {
        Render();
        Sleep(1000);
    }
    return 0; 
}

ID2D1Factory* pFactory = nullptr;
ID2D1HwndRenderTarget* pRenderTarget = nullptr;
ID2D1SolidColorBrush* pBrush = nullptr;

void CreateGraphicsResources(HWND hWnd)
{
    if (pRenderTarget == nullptr)
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        // 创建渲染目标
        HRESULT hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, size),
            &pRenderTarget);

        // 创建画刷
        if (SUCCEEDED(hr))
        {
            pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::SkyBlue, 0.5f),  // 50% 透明的天空蓝色
                &pBrush
            );
        }
    }
}

void DiscardGraphicsResources()
{
    if (pBrush) pBrush->Release();
    if (pRenderTarget) pRenderTarget->Release();
    pBrush = nullptr;
    pRenderTarget = nullptr;
}


#include <shellapi.h>  // For Shell_NotifyIcon

#define WM_TRAYICON (WM_USER + 1)  // 自定义消息，用于托盘图标事件
#define ID_TRAY_EXIT 1001          // 菜单选项：退出
#define ID_TRAY_ABOUT 1002         // 菜单选项：关于

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddTrayIcon(HWND hWnd);
void RemoveTrayIcon(HWND hWnd);
void ShowTrayMenu(HWND hWnd);
void OnAbout(HWND hWnd);
void OnExit();

// 托盘图标 ID
NOTIFYICONDATA nid;
#include "resource.h"

// 添加托盘图标
void AddTrayIcon(HWND hWnd)
{
    memset(&nid, 0, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;  // 图标 ID
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;  // 自定义消息，用于托盘图标事件
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));  // 使用默认应用程序图标
    wcscpy_s(nid.szTip, L"GPUTempMonD3D");

    Shell_NotifyIcon(NIM_ADD, &nid);  // 添加托盘图标
}

// 移除托盘图标
void RemoveTrayIcon(HWND hWnd)
{
    Shell_NotifyIcon(NIM_DELETE, &nid);  // 从托盘中删除图标
}

// 显示托盘菜单
void ShowTrayMenu(HWND hWnd)
{
    POINT pt;
    GetCursorPos(&pt);  // 获取鼠标当前位置

    // 创建菜单
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_ABOUT, L"About");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    // 设置菜单为前台窗口
    SetForegroundWindow(hWnd);

    // 显示菜单
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);

    DestroyMenu(hMenu);  // 销毁菜单
}

// 显示“关于”对话框
void OnAbout(HWND hWnd)
{
    MessageBox(hWnd, L"GPUTempMon D3D\nVersion 1.0 by MJ0011", L"About GPUTempMon D3D", MB_OK | MB_ICONINFORMATION);
}

// 退出程序
void OnExit()
{
    PostQuitMessage(0);
}



static bool isVisible = true;
// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT hr;
    switch (message)
    {

    case WM_PAINT:
       hr = DefWindowProc(hWnd, message, wParam, lParam);
       Render();
       return hr; 


    case WM_DESTROY:
        DiscardGraphicsResources();
        PostQuitMessage(0); 
        break;

    case WM_HOTKEY:
        if (wParam == 1)  // 热键 ID 为 1
        {
            

            // 如果按下 Ctrl + Shift + P，切换窗口的显示状态
            if (isVisible)
            {

                ShowWindow(hWnd, SW_HIDE);  // 隐藏窗口
                isVisible = false;
            }
            else
            {
                ShowWindow(hWnd, SW_SHOW);  // 显示窗口
                isVisible = true;
            }

            Render();
        }
        break;
    case WM_TRAYICON:
        // 处理托盘图标的鼠标事件
        if (lParam == WM_RBUTTONUP)  // 右键弹起时显示菜单
        {
            ShowTrayMenu(hWnd);
        }
        break;

    case WM_COMMAND:
        // 处理菜单点击事件
        switch (LOWORD(wParam))
        {
        case ID_TRAY_ABOUT:
            OnAbout(hWnd);  // 显示关于对话框
            break;

        case ID_TRAY_EXIT:
            OnExit();  // 退出程序
            break;
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void InitTextFormat()
{
    // 创建 DirectWrite 工厂
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWriteFactory));

    // 创建文本格式
    hr = g_pDWriteFactory->CreateTextFormat(
        L"Arial",                // 字体
        nullptr,                 // 字体系列
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        12.0f,                   // 字体大小
        L"en-us",                // 语言
        &g_pTextFormat
    );
}

// WinMain 函数
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow
)
{

    // 注册窗口类
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"Direct3DWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    // 创建窗口
    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST |        // 窗口总是位于最上层
        WS_EX_LAYERED | WS_EX_TRANSPARENT,     // 使窗口鼠标点击穿透
        L"Direct3DWindowClass", // 窗口类名
        L"Overlay",            // 窗口标题
        WS_POPUP,              // 无边框窗口
        0, 0,                  // 窗口位置（左上角）
        200, 150, // 窗口大小（全屏）
        nullptr,               // 父窗口
        nullptr,               // 菜单
        hInstance,             // 程序句柄
        nullptr                // 额外参数
    );
    // 设置窗口透明背景

    // 设置窗口透明度
    SetLayeredWindowAttributes(hWnd, 0, 180, LWA_ALPHA);
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

    // 初始化 Direct3D
    if (FAILED(InitD3D(hWnd)))
    {
        MessageBox(hWnd, L"failed d3d", L"", MB_OK);

        CleanupDevice();

        return 0;
    }
    // 初始化 Direct3D
    if (FAILED(InitD2D(hWnd)))
    {

        CleanupDevice();
        return 0;
    }
    InitTextFormat();
    // 初始化 Direct2D 工厂
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);



    // 在窗口初始化时注册 Ctrl + Shift + X 为全局热键
    RegisterHotKey(hWnd, 1, MOD_CONTROL | MOD_SHIFT, 'P');

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);  // 更新窗口的透明内容

    // 添加托盘图标
    AddTrayIcon(hWnd);

    ReadyForGPU();


    DWORD tid;

    CreateThread(NULL, 0, RenderThread, 0, 0, &tid);
    // 主消息循环
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }

    // 移除托盘图标
    RemoveTrayIcon(hWnd);

    // 清理
    CleanupDevice();
    // 在程序退出时注销热键
    UnregisterHotKey(hWnd, 1);


    return (int)msg.wParam;
}