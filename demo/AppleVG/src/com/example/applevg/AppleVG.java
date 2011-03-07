package com.example.applevg;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;


public class AppleVG extends Activity {

	/** Called when the activity is first created. */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		mGLView = new DemoGLSurfaceView(this, metrics.widthPixels, metrics.heightPixels);
		setContentView(mGLView);
	}

	@Override
	protected void onPause() {
		super.onPause();
		this.finish();
	}
	
	@Override
	protected void onDestroy() {
		android.os.Process.killProcess(android.os.Process.myPid());
		nativeDestroy();
		System.exit(0);
	}
	
	private GLSurfaceView mGLView;

	private native void nativeDestroy();
	
	static {
		System.loadLibrary("openvg-apple");
	}
}

class DemoGLSurfaceView extends GLSurfaceView {
	static final String LOG_TAG = "DemoGLSurfaceView";
	
	public DemoGLSurfaceView(Context context, int width, int height) {
		super(context);
		mRenderer = new DemoRenderer();
		setRenderer(mRenderer);
		
		mRenderer.setWidth(width);
		mRenderer.setHeight(height);
		
		Log.d(LOG_TAG, "(w,h)=(" + width + "," + height + ")");
	}   

	public boolean onTouchEvent(final MotionEvent event) {
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			//nativeClick(event.getX(), event.getY());
			break;
		case MotionEvent.ACTION_MOVE:
			//nativeDrag(event.getX(), event.getY());
			break;
		}
		
		//this.requestRender();

		return true;
	}

	DemoRenderer mRenderer;
	
	//private native void nativeClick(float x, float y);
	//private native void nativeDrag(float x, float y);
}

class DemoRenderer implements GLSurfaceView.Renderer {
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
