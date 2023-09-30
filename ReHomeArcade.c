#include <psp2/kernel/modulemgr.h>
#include <taihen.h>
#include <string.h>
#include <vitasdk.h>
#include <stdarg.h>


static SceUID xml_bypass;
static SceUID xml_bypass2;

static tai_hook_ref_t xml_bypass2_ref;
static tai_hook_ref_t xml_bypass_ref;

static int has_free = 0;
static int has_demo = 0;

int xml_get_key(int handle, char *key) {
    SceUID ret;
    
    ret = TAI_CONTINUE(int, xml_bypass_ref, handle, key);
  
    if(!sceClibStrcmp(key, "free")) {
        sceClibPrintf("[ReHomeArcade] key %s\n", key);
        has_free = 1;
        return 1;
    }
    
    if(!sceClibStrcmp(key, "demo_mode")) {
        sceClibPrintf("[ReHomeArcade] key %s\n", key);
        has_demo = 1;
        return 0;
    }
    
    return ret;
}

int xml_get_key_value(int handle, int val) {
    SceUID ret;
    
    ret = TAI_CONTINUE(int, xml_bypass2_ref, handle, val);
  
    if(has_free) {
        has_free = 0;
        return 1;
    }
    
    if(has_demo) {
        has_demo = 0;
        return 0;
    }
    
    return ret;
}


void _start() __attribute__ ((weak, alias ("module_start"))); 
void module_start(SceSize argc, const void *args) {
    sceClibPrintf("[ReHomeArcade] START\n");
    
    tai_module_info_t tai_info;
    tai_info.size = sizeof(tai_module_info_t);
    
    taiGetModuleInfo(TAI_MAIN_MODULE, &tai_info);

    xml_bypass = taiHookFunctionOffset(&xml_bypass_ref, tai_info.modid, 0, 0xE06, 1, xml_get_key);
    xml_bypass2 = taiHookFunctionOffset(&xml_bypass2_ref, tai_info.modid, 0, 0x11A0, 1, xml_get_key_value);
}

int module_stop(SceSize argc, const void *args) {
    taiHookRelease(xml_bypass, xml_bypass_ref);
    taiHookRelease(xml_bypass2, xml_bypass2_ref);

    return SCE_KERNEL_STOP_SUCCESS;
}
