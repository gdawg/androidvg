/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.tigervg;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnLongClickListener;



public class TigerVG extends Activity {
	static final String LOG_TAG = "TigerVG";  
	
	/** Called when the activity is first created. */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		
		
		mGLView = new DemoGLSurfaceView(this, metrics.widthPixels, metrics.heightPixels);
		Log.d(LOG_TAG, "After DemoGLSurfaceView");

		setContentView(mGLView);
	}
	
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		this.finish();
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		android.os.Process.killProcess(android.os.Process.myPid());
		nativeDestroy();
		System.exit(0);  
	}  

	private GLSurfaceView mGLView;
	private native void nativeDestroy();

	static {
		System.loadLibrary("openvg-tiger");
	}

}

class DemoGLSurfaceView extends GLSurfaceView implements OnLongClickListener {
	static final String LOG_TAG = "DemoGLSurfaceView";  
	private boolean cont = false;
	
	public DemoGLSurfaceView(Context context, int width, int height) {
		super(context);
		mRenderer = new DemoRenderer();
		setRenderer(mRenderer);
		
		mRenderer.setWidth(width);
		mRenderer.setHeight(height);
		
		Log.d(LOG_TAG, "(w,h)=(" + width + "," + height + ")");
		   
		//this.setRenderMode(RENDERMODE_WHEN_DIRTY);
		this.setOnLongClickListener(this);
		
	}   
   
	@Override
	public boolean onLongClick(View v) {
		if(cont)
		{
			this.setRenderMode(RENDERMODE_CONTINUOUSLY);
			cont=false;
		}
		else
		{
			this.setRenderMode(RENDERMODE_WHEN_DIRTY);
			cont=true;
		}
		Log.d(LOG_TAG, "long click");
		return true;
	} 
	
	public boolean onTouchEvent(final MotionEvent event) {
		if (event.getAction() == MotionEvent.ACTION_DOWN) {

		}
		return super.onTouchEvent(event);
	}   
	//private native int nativeChmod();
	DemoRenderer mRenderer;
}

class DemoRenderer implements GLSurfaceView.Renderer {
	static final String LOG_TAG = "TigerVG";  

	private int width;
	private int height;
	
	public DemoRenderer() {
		this.width = 320;
		this.height = 480;
	}
	
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		nativeStart(width, height);
	}   

	

	public void onSurfaceChanged(GL10 gl, int w, int h) {
	}   

	public void onDrawFrame(GL10 gl) {
		nativeDisplay();
	}
	
	public void setWidth(int width) {
		this.width = width;
	}

	public void setHeight(int height) {
		this.height = height;
	}
			    
	private native int nativeStart(int width, int height);
	private native int nativeDisplay();
}


