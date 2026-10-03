#include <stdio.h>
#include <alsa/asoundlib.h>
#include <aubio/aubio.h>
#include <math.h>

const char* get_note_name(float freq) {
    if (freq < 20) return "---"; // Ignore extremely low noise
    
    // Musical notes array
    const char* notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    // Formula: n = 12 * log2(freq / 440) + 57
    int n = round(12 * log2(freq / 440.0) + 57);
    
    return notes[n % 12];
}

int main() {
    // 1. Setup ALSA
    snd_pcm_t *pcm_handle;
    snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_set_params(pcm_handle, SND_PCM_FORMAT_FLOAT_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 1, 44100, 1, 50000);

    // 2. Setup Aubio
    uint_t win_s = 2048;      // Window size
    uint_t hop_s = 512;       // Hop size
    fvec_t *in = new_fvec(hop_s);
    fvec_t *out = new_fvec(1);
    aubio_pitch_t *pitch = new_aubio_pitch("yin", win_s, hop_s, 44100);

    // 3. Main Loop
    while (1) {
        // Read raw audio from ALSA
        snd_pcm_readi(pcm_handle, in->data, hop_s);

        // Detect pitch
        aubio_pitch_do(pitch, in, out);

        float freq = out->data[0];
        if (freq > 0) {
            const char* note = get_note_name(freq);
            printf("\rPitch: %8.2f Hz | Note: %s   ", freq, note);
            fflush(stdout);
        }
    }

    // Cleanup
    del_aubio_pitch(pitch);
    del_fvec(in);
    del_fvec(out);
    snd_pcm_close(pcm_handle);
    return 0;
}
