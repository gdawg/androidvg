/*
 * Copyright (c) 2010 Yi-Cheng Chen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library in the file COPYING;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
package com.example.starvg;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.view.View;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View.OnLongClickListener;
import android.view.Window;
import android.view.WindowManager;

public class StarVG extends Activity {
	static final String LOG_TAG = "StarVG";

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
	public void onBackPressed() {
		nativeCycleCapsType();
		mGLView.requestRender();
	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event) {
		nativeCycleJoinsType();
		mGLView.requestRender();

		return true;
	}

	@Override
	protected void onDestroy() {
		android.os.Process.killProcess(android.os.Process.myPid());
		nativeDestroy();
		System.exit(0);
	}

	private GLSurfaceView mGLView;

	private native void nativeDestroy();
	private native void nativeCycleCapsType();
	private native void nativeCycleJoinsType();

	static {
		System.loadLibrary("openvg-star");
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

		this.setRenderMode(RENDERMODE_WHEN_DIRTY);

		this.setOnLongClickListener(new OnLongClickListener() {
			@Override
			public boolean onLongClick(View v) {
				Log.d(LOG_TAG, "long click");
				return true;
			}
		});

		this.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				nativeChangeMode();
			}
		});
	}

	@Override
	public boolean onTouchEvent(final MotionEvent event) {
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			nativeClick(event.getX(), event.getY());
			break;
		case MotionEvent.ACTION_MOVE:
			nativeDrag(event.getX(), event.getY());
			break;
		case MotionEvent.ACTION_UP:
			break;
		}

		this.requestRender();

		return super.onTouchEvent(event);
	}

	DemoRenderer mRenderer;

	private native void nativeClick(float x, float y);
	private native void nativeDrag(float x, float y);
	private native void nativeChangeMode();

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
