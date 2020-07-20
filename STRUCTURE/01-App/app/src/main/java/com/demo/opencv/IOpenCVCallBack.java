package com.demo.opencv;

import android.graphics.Bitmap;

public interface IOpenCVCallBack {
    /**
     * 得到 结果
     */
    void OpenCVCallBack(double x, double y, double z, Bitmap m);
}
