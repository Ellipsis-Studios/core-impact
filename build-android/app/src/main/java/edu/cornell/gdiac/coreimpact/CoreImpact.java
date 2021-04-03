package edu.cornell.gdiac.shipdemo;

import android.os.Bundle;

import org.libsdl.app.SDLActivity;

/**
 * Default application entry point
 */
public class CoreImpact extends SDLActivity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
	    // Make sure this is before calling super.onCreate
	    setTheme(R.style.CUGLTheme);
	    super.onCreate(savedInstanceState);
	}
	
}
