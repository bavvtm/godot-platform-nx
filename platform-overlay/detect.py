import os
import sys
from typing import TYPE_CHECKING

from methods import print_error

if TYPE_CHECKING:
  from SCons.Script.SConscript import SConsEnvironment

def get_name():
  return "Switch"

def can_build():
  dkp = os.environ.get("DEVKITPRO", "/opt/devkitpro")
  if not os.path.exists(dkp):
    print_error("DEVKITPRO not found. Switch disabled.")
    return False

  if not os.path.exists("{}/devkitA64".format(dkp)):
    print_error("devkitA64 not found. Switch disabled.")
    return False
  
  if not os.path.exists("{}/libnx".format(dkp)):
    print_error("libnx not found. Switch disabled.")
    return False

  if not os.path.exists("{}/portlibs/switch/bin/aarch64-none-elf-pkg-config".format(dkp)):
    print_error("aarch64-none-elf-pkg-config not found. Switch disabled.")
    return False

  if os.system("pkg-config --version > /dev/null"):
    print_error("pkg-config not found. Switch disabled.")
    return False
  
  return True

def get_opts():
  from SCons.Variables import BoolVariable, EnumVariable, PathVariable
  
  return [
    BoolVariable("use_lto", "Use LTO (Link Time Optimization)", False),
    BoolVariable("touch", "Enable touch events", True),
    PathVariable(
      "mesa_sdk",
      "Path to custom Mesa Switch SDK prefix (containing include/ and lib/)",
      os.environ.get("MESA_SWITCH_SDK", ""),
      PathVariable.PathAccept,
    ),
  ]
  
def get_doc_classes():
  return []

def get_doc_path():
  return "doc_classes"

def get_flags():
  return {
    "arch": "arm64",
    "target": "template_debug",
    "supported": ["threading", "vulkan", "opengl3"],
    "disable_path_overrides": False,
    "vulkan": True,
    "use_volk": False,
    "opengl3": True,
    "alsa": False,
    "audren": True,
		"pulseaudio": False,
    "builtin_pcre2_with_jit": False,
    "module_raycast_enabled": False,
    "module_jolt_physics_enabled": False, # disabled for now
    "module_lightmapper_rd_enabled": False,
    "module_openxr_enabled": False,
    "module_webxr_enabled": False,
		"module_mobile_vr_enabled": False,
		"module_camera_enabled": False,
		"module_mono_enabled": False,
  }
  
def configure(env: "SConsEnvironment"):
  dkp = os.environ.get("DEVKITPRO", "/opt/devkitpro")
  bin_path = "{}/devkitA64/bin".format(dkp)
  
  env["CC"] = "{}/aarch64-none-elf-gcc".format(bin_path)
  env["CXX"] = "{}/aarch64-none-elf-g++".format(bin_path)
  env["LINK"] = env["CXX"]
  env["ENV"]["DEVKITPRO"] = dkp
  updated_path = "{}/portlibs/switch/bin:{}:".format(dkp, bin_path) + os.environ["PATH"]
  env["ENV"]["PATH"] = updated_path
  os.environ["PATH"] = updated_path  # os environment has to be updated for subprocess calls

  arch = ["-march=armv8-a+crc+crypto", "-mtune=cortex-a57", "-mtp=soft", "-fPIE"]
  env.Prepend(CCFLAGS=arch + ["-ffunction-sections", "-fdata-sections", "-ffp-contract=off", "-pipe"])
  
  platform_path = os.path.join("platform", "switch")
  env.Prepend(CPPPATH=[f"{dkp}/portlibs/switch/include", f"{dkp}/libnx/include"])
  env.Prepend(CPPFLAGS=["-isystem", "{}/libnx/include".format(dkp)])
  env.Prepend(LIBPATH=[f"{dkp}/portlibs/switch/lib", f"{dkp}/libnx/lib"])
  env.Prepend(CCFLAGS=["-include", "{}/shim/posix_stubs.h".format(platform_path)])
  env.Prepend(LINKFLAGS=arch + [f"-specs={dkp}/libnx/switch.specs", "-Wl,--gc-sections"])
  
  env.Append(CPPDEFINES=[
    "UNIX_ENABLED",
    "LIBC_FILEIO_ENABLED",
    "PTHREAD_ENABLED",
    "PTHREAD_NO_RENAME",
    ("_FILE_OFFSET_BITS", 64),
    ('MBEDTLS_USER_CONFIG_FILE', f'\\"{platform_path}/mbedtls_config_switch.h\\"')
  ])

  mesa_sdk = env.get("mesa_sdk", "")
  if mesa_sdk:
    mesa_sdk = os.path.abspath(mesa_sdk)
    mesa_inc = os.path.join(mesa_sdk, "include")
    mesa_lib = os.path.join(mesa_sdk, "lib")
    mesa_pc = os.path.join(mesa_lib, "pkgconfig")
    env.Prepend(CPPPATH=[mesa_inc])
    env.Prepend(LIBPATH=[mesa_lib])
    existing_pc = env["ENV"].get("PKG_CONFIG_PATH", os.environ.get("PKG_CONFIG_PATH", ""))
    updated_pc = f"{mesa_pc}:{existing_pc}" if existing_pc else mesa_pc
    env["ENV"]["PKG_CONFIG_PATH"] = updated_pc
    os.environ["PKG_CONFIG_PATH"] = updated_pc
  
  if env["target"] == "template_release":
    env.Prepend(CCFLAGS=["-Os"])

  elif env["target"] == "template_debug":
    env.Prepend(CCFLAGS=["-O2", "-g3"])
    env.Append(CPPDEFINES=[
      "DEBUG_ENABLED",
      "DEBUG_MEMORY_ENABLED",
    ])

  if env["use_lto"]:
    env.Append(CCFLAGS=["-flto"])
    if env.GetOption("num_jobs") > 1:
      env.Append(LINKFLAGS=["-flto=" + str(env.GetOption("num_jobs"))])
    else:
      env.Append(LINKFLAGS=["-flto"])
      
    env["RANLIB"] = "{}/aarch64-none-elf-gcc-ranlib".format(bin_path)
    env["AR"] = "{}/aarch64-none-elf-gcc-ar".format(bin_path)
  else:
    env["RANLIB"] = "{}/aarch64-none-elf-ranlib".format(bin_path)
    env["AR"] = "{}/aarch64-none-elf-ar".format(bin_path)

  env.Append(CCFLAGS=["-pipe"])
  env.Append(LINKFLAGS=["-pipe"])
    
  if env.get("touch", True):
    env.Append(CPPDEFINES=["TOUCH_ENABLED"])
    
  if env.get("vulkan", True):
    env.Append(CPPDEFINES=["VULKAN_ENABLED", "RD_ENABLED", "VK_USE_PLATFORM_VI_NN"])
    if not env["use_volk"]:
      env.Append(LIBS=["vulkan"])

  if env.get("opengl3", True):
    env.Append(CPPDEFINES=["GLES3_ENABLED"])
    env.Append(LIBS=["EGL", "GLESv2", "glapi"])
    
  if env.get("audren", True):
    env.Append(CPPDEFINES=["AUDREN_ENABLED"])
    
  env.Prepend(CPPPATH=[f"#{platform_path}", f"#{platform_path}/shim"])
  env.Append(
    CPPDEFINES=[
      "__SWITCH__",
      "HOMEBREW_ENABLED",
      "HORIZON_ENABLED",
      "DEFAULT_ROMFS_PATH='\"romfs:/\"'",
      "DEFAULT_USER_DATA_PATH='\"sdmc:/switch/godot\"'"
    ]
  )
  
  env.Append(LIBS=["expat", "zstd", "z", "nx", "m"])
