/** \file
 * Entry points into the firmware image.
 *
 * These are the functions that we can call from our tasks
 * in the Canon 1.1.3 firmware for the 5D Mark III.
 *
 */
/*
 * Copyright (C) 2012 Magic Lantern Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include <stub.h>

.text

NSTUB( ROMBASEADDR, firmware_entry )

#define RAM_OFFSET 0xFF9DEF48 // some functions are copied to RAM at around ff0c009c; they have to be called from RAM...


NSTUB(0x23D08, pre_isr_hook)
NSTUB(0x23D0C, post_isr_hook)

/** Startup **/
NSTUB(0xFF0C1C0C, cstart)
NSTUB(0xFF9FB390 - RAM_OFFSET, bzero32)
NSTUB(0xff9e1e00 - RAM_OFFSET, create_init_task)
NSTUB(0xff0c520c, init_task)
NSTUB(   0x468b8, additional_version)

/** Misc **/
NSTUB(0xC0220000, camera_engine)
NSTUB(0xff135f10, LoadCalendarFromRTC)
NSTUB(0xff9f4f3c - RAM_OFFSET, vsnprintf)

/** File I/O **/
NSTUB(0xff309994, _FIO_CreateFile)
NSTUB(0xff3098d8, _FIO_Open) // or OpenFile?
NSTUB(0xff309d38,  FIO_CloseFile)
NSTUB(0xff309c68,  FIO_SeekFile)
NSTUB(0xff309ae8,  FIO_ReadFile)
NSTUB(0xff309c88,  FIO_WriteFile)
NSTUB(0xff309eb4, _FIO_GetFileSize)
NSTUB(0xff309a40, _FIO_RemoveFile)
NSTUB(0xff30a594, _FIO_CreateDirectory)
NSTUB(0xff30ab5c, _FIO_FindFirstEx)
NSTUB(0xff30ac50,  FIO_FindNextEx)
NSTUB(0xff30ad30,  FIO_CleanupAfterFindNext_maybe) // proper name: FindClose

/** Bitmap **/
NSTUB(   0x42d7c, bmp_vram_info )                 // look in SetBitmapVramAddress - as in 60D
NSTUB(   0x4157c, LCD_Palette) // in InitializeBitmapDisplayDevice, right after 0xc0f14800

/** Debug messages **/
NSTUB(   0x24b60, dm_names)
NSTUB(0xff9e4ad8 - RAM_OFFSET, DryosDebugMsg)
NSTUB(0xff9e4e40 - RAM_OFFSET, dm_set_store_level)
NSTUB(0xff9e4dd8 - RAM_OFFSET, dm_set_print_level)

/** Task sleep **/
NSTUB(0xff9e79a4 - RAM_OFFSET, msleep)

/** Semaphores **/
NSTUB(0xff9e7368 - RAM_OFFSET, create_named_semaphore)
NSTUB(0xff9e74c8 - RAM_OFFSET, take_semaphore)
NSTUB(0xff9e75b4 - RAM_OFFSET, give_semaphore)

/** Recursive locks **/
NSTUB(0xff9eb2ec - RAM_OFFSET, CreateRecursiveLock)
NSTUB(0xff9f3900 - RAM_OFFSET, AcquireRecursiveLock) // AJ_KernelDry_KerRLock.c
NSTUB(0xff9f3a14 - RAM_OFFSET, ReleaseRecursiveLock) // AJ_KernelDry_KerRLock.c_p2

/** Message queues **/
NSTUB(0xff9eb104 - RAM_OFFSET, msg_queue_create)
NSTUB(0xff9f37b4 - RAM_OFFSET, msg_queue_post)
NSTUB(0xff9f35c8 - RAM_OFFSET, msg_queue_receive)

/** Tasks **/
NSTUB(0xff9e7a58 - RAM_OFFSET, task_create)
NSTUB(   0x23d00, task_dispatch_hook )
NSTUB(0xff9ecd70 - RAM_OFFSET, task_trampoline)

/** Task info **/
NSTUB(   0x25114, task_max)
NSTUB(0xff9e1474 - RAM_OFFSET, get_current_task)
NSTUB(0xff9f8860 - RAM_OFFSET, is_taskid_valid) // AJ_task_trampoline_related_p10
NSTUB(0xff9ef87c - RAM_OFFSET, get_obj_attr) // AJ_checks_if_Process_id_created_by_Dryos

/** Interrupts **/
NSTUB(0xff0c10e0, cli_save)
NSTUB(0xff0c10f4, sei_restore)

/** Eventprocs (call by name) **/
NSTUB(0xff13b68c, call)

/** GUI **/
NSTUB(0xFF0DEC8C, GUI_Control)
NSTUB(0xff188c88, SetGUIRequestMode)
NSTUB(0xFF0DE8BC, gui_main_task)
NSTUB(   0x24118, gui_main_struct)
NSTUB(0xff0df2e4, gui_init_end)
NSTUB(0xff1a4fc8, gui_massive_event_loop)
NSTUB(   0x32244, gui_task_list)

/** Dialog API **/
NSTUB(0xff44c8c4, CreateDialogBox)
NSTUB(0xff44b894, DeleteDialogBox)
NSTUB(0xff44d5f0, dialog_redraw)
NSTUB(0xff3528a8, dialog_set_property_str)
NSTUB(0xff46d03c, RedrawDisplay)

/** Dialog handlers **/
NSTUB(0xff658aa8, LiveViewLevelApp_handler)
NSTUB(0xFF623240, LiveViewWbApp_handler)
NSTUB(0xff4afe08, LiveViewApp_handler)
NSTUB(0xff36728c, PlayMain_handler)
NSTUB(0xff4cba10, PlayMovieGuideApp_handler)
	NSTUB(0xff49c124, ErrCardForLVApp_handler) // not sure
NSTUB(0xff49cc68, ErrForCamera_handler) // ERR70, ERR80 etc (DlgErrForCamera.c, AJ_DIALOG.HANDLER_DlgErrForCamera.c)
NSTUB(0xff4aba48, HideUnaviFeedBack_maybe) // look for StartUnaviFeedBackTimer
NSTUB(0xff4b6930, ShootOlcApp_handler) // AJ_DIALOG.HANDLER_DlgShootOlcInfo.c

/** Properties **/
NSTUB(0xff12340c, prop_register_slave)
NSTUB(0xff123c88, _prop_cleanup)
NSTUB(0xff123bbc, prop_deliver)
NSTUB(0xff123600, _prop_request_change)
NSTUB(0xff123de4, _prop_get_value) // unsafe

/** Memory allocation **/
NSTUB(0xff0c80ec, malloc)
NSTUB(0xFF0C8144, free) // see "can't malloc"
NSTUB(0xff9e88e8 - RAM_OFFSET, AllocateMemory)
NSTUB(0xff9e8c84 - RAM_OFFSET, FreeMemory)
NSTUB(0xff9f6ee4 - RAM_OFFSET, alloc_dma_memory)
NSTUB(0xff9f6f18 - RAM_OFFSET, free_dma_memory)
NSTUB(0xFF9E8688 - RAM_OFFSET, GetMemoryInformation)

/** ExMem **/
NSTUB(0xff13c134, AllocateMemoryResource) // m_pfAllocMemoryCBR
NSTUB(0xff13c180, AllocateContinuousMemoryResource) // m_pfContAllocMemoryCBR
NSTUB(0xff13c298, FreeMemoryResource) // m_pfFreeMemoryCBR

NSTUB(0xFF9EA118 - RAM_OFFSET, GetFirstChunkFromSuite)
NSTUB(0xFF9EA8B4 - RAM_OFFSET, GetNextMemoryChunk)
NSTUB(0xFF9E9AF0 - RAM_OFFSET, GetMemoryAddressOfMemoryChunk)

/** Engio **/
NSTUB(0xff28cc3c, _engio_write)
NSTUB(0xff28c9c8, shamem_read) // AJ_0x8FB0_engio_struct_n_R0_manipulation_to_get_ptr
NSTUB(0xff28c92c, _EngDrvOut) // AJ_EngDrvOut_1xVar_to_ShadowStruct

/** PTP **/
NSTUB(0xff3f4f54, ptp_register_handler)

/** Audio **/
NSTUB(0xff136f5c, _audio_ic_read)
NSTUB(0xff136ea4, _audio_ic_write) // str:Reg_0x_02X_Data_04x

/** ASIF **/
NSTUB(0xff13790c, PowerAudioOutput)
NSTUB(0xff10ec58, StartASIFDMADAC)
NSTUB(0xff10eaa0, StartASIFDMAADC)
NSTUB(0xff10ed30, StopASIFDMADAC)
NSTUB(0xff10f1d4, SetNextASIFADCBuffer)
NSTUB(0xFF10F288, SetNextASIFDACBuffer)
NSTUB(0xff1379a4, SetSamplingRate )
NSTUB(0xff1370a4, SetAudioVolumeOut)

/** experimental EDMAC research **/
NSTUB(0xAE24, CreateMemorySuite)
NSTUB(0xAC1C, DeleteMemorySuite)


/** EDMAC routines */
NSTUB(0x000125F8, SetEDmac)
NSTUB(0x00012910, StartEDmac)
//NSTUB(0x12B38, PopEDmac)
NSTUB(0x00012AA4, PopEDmac)
NSTUB(0x00012A5C, AbortEDmac)
NSTUB(0x000126A4, ConnectWriteEDmac)
NSTUB(0x00012768, ConnectReadEDmac)

/** keep the old name until all are refcatored **/
NSTUB(0x00012AFC, EDMAC_RegisterCompleteCBR)

/** register/unregister CBR names **/
NSTUB(0x00012AFC, RegisterEDmacCompleteCBR)
NSTUB(0x00012BE4, RegisterEDmacAbortCBR)
NSTUB(0x00012C78, RegisterEDmacPopCBR)
NSTUB(0x00012B38, UnregisterEDmacCompleteCBR)
NSTUB(0x00012C20, UnregisterEDmacAbortCBR)
NSTUB(0x00012CB4, UnregisterEDmacPopCBR)


NSTUB(0xFF576634, PackMem_RegisterEDmacCompleteCBRForMemorySuite)
NSTUB(0xFF5766D0, PackMem_RegisterEDmacPopCBRForMemorySuite)
NSTUB(0xFF576348, PackMem_SetEDmacForMemorySuite)
NSTUB(0xFF576564, PackMem_PopEDmacForMemorySuite)
NSTUB(0xFF57645C, PackMem_StartEDmac)

/** ResLock */
NSTUB(0xff290b94, CreateResLockEntry)
NSTUB(0xff29105c, LockEngineResources)
NSTUB(0xff291200, UnLockEngineResources)


/** Making the card bootable (not working on 5D3) **/
// NSTUB(0xff6ab324, cfReadBlk)
// NSTUB(   0x20890, cf_device) idk...
// NSTUB(0xff6b16ac, sdReadBlk)
// NSTUB(   0x399D4, sd_device)
// NSTUB(0xff5bb2c8, fsuDecodePartitionTable) // AJ_fsuGetPart_related

/* Electronic level - roll/pitch info */
NSTUB(0xFF272050, GUI_SetRollingPitchingLevelStatus); // RequestRollingPitching - no side effects :)
// NSTUB(0xFF195084, GUI_SetRollingPitchingLevelStatus) // has side effects

/** Unused **/
// NSTUB(0x23D08, pre_isr_hook)
// NSTUB(0x23D0C, post_isr_hook)
// NSTUB(0xff181314, ENCODE_StartEncodeJpeg)
// NSTUB(0xFF9EB650 - RAM_OFFSET, dma_memcpy)
// NSTUB(0xff1064c8, PD_RemoteRelease)
// NSTUB(0xff144a3c, copy_malloc_info_maybe)
// NSTUB(0xff9b1be0 - RAM_OFFSET, LightMeasure_n_Callback_r0) - looks a bit different, disabling for now
// NSTUB(0xFF0DEE48, GUI_ChangeMode)
// NSTUB(0xff19d7e8, GUI_GetFirmVersion)
// NSTUB(0xff44a668, ctrlman_dispatch_event)
// NSTUB(0xff2f3f8c, MuteOff_0)
// NSTUB(0xff2f3f44, MuteOn_0)
// NSTUB(0xff12e110, SetBitmapVramAddress)
// NSTUB(0xff44a7b8, gui_task_create)
// NSTUB(0xFF44A870, gui_task_destroy)
// NSTUB(0xff4b09e4, LiveViewApp_handler_BL_JudgeBottomInfoDispTimerState)
// NSTUB(0xff4b1cc4, LiveViewApp_handler_end) // start of next function
// NSTUB(0xff13b630, register_func)
// NSTUB(0xFF9E9AC0 - RAM_OFFSET, GetSizeOfMemoryChunk)
// NSTUB(0xFF9E9A90 - RAM_OFFSET, GetRemainOfMemoryChunk)
// NSTUB(0xFF9E9ED8 - RAM_OFFSET, GetNumberOfChunks)
// NSTUB(0xFF9E9DF8 - RAM_OFFSET, GetSizeOfMemorySuite)

