
#include "mdi_audio.h"

#define  VS_MM_BUFFER_SIZE	128
#define VS_MM_AUDIO_BUILD_CACHE_SIZE		(2*1024)
#define VS_MM_AUDIO_FRM_RDS_PS_NAME_SIZE	10
#define VS_MM_AUDIO_FRM_RDS_RT_SIZE		64


typedef struct
{
    S32 index;
    S32 num;
    U8 stop;
    S32 count;
    S32 sub_type;
    UI_character_type filename[VS_MM_BUFFER_SIZE];
    UI_character_type record_file_name[VS_MM_BUFFER_SIZE];
    PU8 audio_buffer;
    U32 total_time;
    U32 play_time;
    U8 total_time_str[VS_MM_BUFFER_SIZE];
    U8 play_time_str[VS_MM_BUFFER_SIZE];
    U8 audio_level;
    S8 mute;
    S8 suspended;
    S8 bg_playing;
    U32 cur_offset;
    U32 cache_duration;
    U8 cache_duration_buf[VS_MM_BUFFER_SIZE];
    U32 total_duration;
    U8 total_duration_buf[VS_MM_BUFFER_SIZE];
    U8 audio_cache[VS_MM_AUDIO_BUILD_CACHE_SIZE];
    U8 *audio_cache_file_buf_p;
    U8 *audio_cache_proc_buf_p;
    U32 audio_build_cache_progress;
    S32 magnitude_index;
    S32 reverb_index;
    U8 surround_mode;
    U8 amplitude_val[16];
    U8 top[16];
    S32 speed;
    U16 freq;
    BOOL mono;
    S32 type;
    U8 str_type;
    U16 id_type;
    U32 buffer_size;
    S8 file_buf[VS_MM_BUFFER_SIZE];
    S8 progress_buf[VS_MM_BUFFER_SIZE];
    S8 state_buf[VS_MM_BUFFER_SIZE];
    S8 midi_size_ptr[VS_MM_BUFFER_SIZE];
    mdi_handle aud_handle;
    mdi_result result;
    U32 offset;
    BOOL wait_buffer;
    S32 player_handle;
    BOOL finish;
    PS8 media_buffer;
    UI_character_type prog_name[VS_MM_AUDIO_FRM_RDS_PS_NAME_SIZE];
    UI_character_type radio_text[VS_MM_AUDIO_FRM_RDS_RT_SIZE + 1];
	mdi_handle m_handle032;
	
    U16 m_signalBand[20];
} vs_mm_audio_context_struct;

void Apollo_audio_recorder_start(void);
void Apollo_audio_recorder_stop(void);




