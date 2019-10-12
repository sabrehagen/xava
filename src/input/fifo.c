#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <time.h>
#include "fifo.h"

int rc;

int open_fifo(const char *path)
{
	int fd = open(path, O_RDONLY);
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	return fd;
}


// input: FIFO
void* input_fifo(void* data)
{
	struct audio_data *audio = (struct audio_data *)data;
	int fd;
	int n = 0;
	//signed char buf[1024];
	//int tempr, templ, lo, q;
	int i;
	int t = 0;
	//int size = 1024;
	int bytes = 0;
	int16_t buf[audio->inputsize];
	struct timespec req = { .tv_sec = 0, .tv_nsec = 10000000 };

	fd = open_fifo(audio->source);

	while (1) {
		bytes = read(fd, buf, sizeof(buf));

		if (bytes < 1) { //if no bytes read sleep 10ms and zero shared buffer
			nanosleep (&req, NULL);
			t++;
			if (t > 10) {
				for (i = 0; i < audio->inputsize; i++)
					audio->audio_out_l[i] = 0;

				// prevents filling a invalid buffer whilst in mono mode
				if (audio->channels == 2) {
					for (i = 0; i < audio->inputsize; i++) 
						audio->audio_out_r[i] = 0;
				}
				close(fd);
				fd = open_fifo(audio->source);
				t = 0;
			}
		} else { //if bytes read go ahead
			t = 0;

			// assuming samples are 16bit (as per example)
			// also reading more than the retrieved buffer is considered memory corruption
			for (i = 0; i < bytes/2; i += 2) {
				if (audio->channels == 1) audio->audio_out_l[n] = (buf[i] + buf[i + 1]) / 2;

				//stereo storing channels in buffer
				if (audio->channels == 2) {
					audio->audio_out_l[n] = buf[i];
					audio->audio_out_r[n] = buf[i + 1];
				}

				n++;
				if (n == audio->inputsize-1) n = 0;
			}

/*
			for (q = 0; q < (size / 4); q++) {

				tempr = ( buf[ 4 * q + 3] << 2);

				lo =  ( buf[4 * q + 2] >> 6);
				if (lo < 0)lo = abs(lo) + 1;
				if (tempr >= 0)tempr = tempr + lo;
				else tempr = tempr - lo;

				templ = ( buf[ 4 * q + 1] << 2);

				lo =  ( buf[ 4 * q] >> 6);
				if (lo < 0)lo = abs(lo) + 1;
				if (templ >= 0)templ = templ + lo;
				else templ = templ - lo;

				if (audio->channels == 1) audio->audio_out_l[n] = (tempr + 
templ) / 
2;


				//stereo storing channels in buffer
				if (audio->channels == 2) {
					audio->audio_out_l[n] = templ;
					audio->audio_out_r[n] = tempr;
					}



				n++;
				if (n == 2048 - 1)n = 0;
			}
*/
		}

		if (audio->terminate == 1) {
			close(fd);
			break;
		}
	}
	return 0;
}
