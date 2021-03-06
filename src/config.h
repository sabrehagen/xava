struct config_params {
	char *color, *bcolor, *audio_source, **gradient_colors, *shadow_color;
	double monstercat, integral, gravity, ignore, sens, logScale, logBegin, logEnd,
 eqBalance, foreground_opacity, background_opacity; 
	unsigned int lowcf, highcf, shdw, shdw_col, inputsize, fftsize, gradients, 
 bgcol, col;
	double *smooth;
	int smcount, im, om, autobars, stereo, vsync, fixedbars, framerate, bw, bs,
 autosens, overshoot, waves, w, h;
	char *winA;
	int wx, wy;
	_Bool oddoneout, fullF, transF, borderF, bottomF, winPropF, interactF, taskbarF, iAmRoot;
} p;
void load_config(char *configPath, void* p);
