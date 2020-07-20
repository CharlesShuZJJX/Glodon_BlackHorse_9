package com.demo.opencv;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

//需要给我们提供一个Lisenter 来监听
public class UtilsToService implements IOpenCVCallBack{
    private static final String TAG = "UtilsToService";

    private Bitmap mUpLoadBitmap;
    private Bitmap mDownLoadBitmap;
    private double mLength = 20.5;
    private double mWidth = 30.5;
    private double mArea = 45.5;

    private static UtilsToService sInstance;

    private getResultListener mListener;
    static {//加载so库
        System.loadLibrary("native-lib");
    }

    public static UtilsToService getInstance() {
        if (sInstance == null) {
            sInstance = new UtilsToService();
        }
        return sInstance;
    }

    public void setUpLoadBitmap(Bitmap bitmap) {
        mUpLoadBitmap = bitmap;
        // 可以直接上传给 OPENCV 处理程序
        // 0 mLength
        // 1 mWidth
        // 2 mArea
        // 默认值 [0.0, 0.0, 0.0]
        mDownLoadBitmap = bitmap;
        double[] arr = getEdge(mDownLoadBitmap);
        mLength = arr[0];
        mWidth = arr[1];
        mArea = arr[2];
        //=======================测试，上传图片后删
        if (mListener != null) {
            mListener.getResult();
        }
        //=======================

    }

    public Bitmap getDownLoadBitmap() {
        //得到下载的数据
        return mDownLoadBitmap;
    }
    public String getLength(){
        return String.valueOf(mLength);
    }
    public String getWidth(){
        return String.valueOf(mWidth);
    }
    public String getArea(){
        return String.valueOf(mArea);
    }
    public String getcontent(){
        String result="";
        if(mWidth <= 0.2){
            result = "参照《混凝土结构设计规范》（GB50010-2010）结构的裂缝符合要求";
            return result;
        }
        else{
            result = "参照《混凝土结构设计规范》（GB50010-2010）结构的裂缝不符合要求";
            return result;
        }
    }
    public void addListener(getResultListener listener) {
        mListener = listener;
    }

    /**
     * 处理 传回来的数据
     * @param x
     * @param y
     * @param z
     * @param m
     */
    @Override
    public void OpenCVCallBack(double x, double y, double z, Bitmap m) {
        mLength =x;
        mWidth =y;
        mArea =z;
        mDownLoadBitmap = m;
        if (mListener != null) {
            mListener.getResult();
        }
    }

    public interface getResultListener {
        void getResult();
    }
    //获得Canny边缘
    native double[] getEdge(Object bitmap);
}