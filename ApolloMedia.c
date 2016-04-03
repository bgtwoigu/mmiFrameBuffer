

#include "ApolloMedia.h"

#define MAX_RECORDER_SIZE		0xFFFFFFFF

#define RECORDER_AMR_NAME	"record.amr"

vs_mm_audio_context_struct g_vs_mm_audio_cntx;

void Apollo_audio_misc_record_callback(mdi_result result, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!(result == MDI_AUDIO_SUCCESS || result == MDI_AUDIO_TERMINATED))
    {
        kal_prompt_trace(MOD_MMS, "vs_mm_audio_misc_record_callback, error code :%d;\n", result);
    }

}

void vs_mm_audio_misc_get_file_path(CHAR* path, CHAR* file)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //    S8 fileBuf[VS_MM_BUFFER_SIZE];
    kal_wchar wszAudio[] = {0x0041, 0x0075, 0x0064, 0x0069, 0x006F, 0x005C, 0x0000};  /* default folder = Audio */


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //    memset(fileBuf, 0, VS_MM_BUFFER_SIZE);
    memset(path, 0, VS_MM_BUFFER_SIZE);

    mmi_wcscat((U16*) path, (U16*) AVK_TEST_FILE_PATH);
    mmi_wcscat((U16*) path, (U16*) L"avk_mm_audio\\");
    mmi_wcscat((U16*) path, (U16*) file);
}


void vs_mm_audio_misc_record_file_setting_save(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(g_vs_mm_audio_cntx.record_file_name, 0, sizeof(g_vs_mm_audio_cntx.record_file_name));

    switch (g_vs_mm_audio_cntx.sub_type)
    {
    case 1:
        vs_mm_audio_misc_get_file_path((CHAR*) g_vs_mm_audio_cntx.record_file_name, (CHAR*) L"record.amr");
        break;
    case 2:
        vs_mm_audio_misc_get_file_path((CHAR*) g_vs_mm_audio_cntx.record_file_name, (CHAR*) L"record.wav");
        break;
    default:
        break;
    }
    return;
}


void Apollo_audio_recorder_start(void) {
	mdi_result result;

	vs_mm_audio_misc_record_file_setting_save();
	
	result = mdi_audio_start_record_with_limit(
            (void*) g_vs_mm_audio_cntx.record_file_name,
            MDI_FORMAT_AMR,
            0,      /* 0: Low, >0: High */
            Apollo_audio_misc_record_callback,
            NULL,
            MAX_RECORDER_SIZE,  /* Max record size in byte. 0: unlimited */
            30);    /* Max record time in sec.  0: unlimited */
	 if (result != MDI_AUDIO_SUCCESS) {
	    //kal_prompt_trace(MOD_MMS, "mdi_audio_start_record_with_limit, error code :%d;\n", result);
	    printf("aaaaaaaaaaa\n");
	    return;
	} else {
	    //gui_start_timer(1000, vs_mm_audio_009_get_record_time_timer_callback);
	    printf("bbbbbbbbbbb\n");
	}

}

void Apollo_audio_recorder_stop(void) {
	result = mdi_audio_stop_record();

    if (result == MDI_AUDIO_SUCCESS)
    {
        mdi_audio_play_file((void*) g_vs_mm_audio_cntx.record_file_name, DEVICE_AUDIO_PLAY_ONCE, NULL, NULL, NULL);
    }
}




