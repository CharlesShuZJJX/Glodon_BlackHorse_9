package com.demo.opencv;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.widget.EditText;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

public class ShowImageActivity extends AppCompatActivity {

    private static final String TAG = "ShowImageActivity";
    private EditText mWidth ;
    private EditText mHeight;
    private EditText mArea;
    private ImageView mImag;
    private EditText mcontent;
    private Bitmap bitmap;
    private byte[] data;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_image);
        init();

        //========================================


//        data = getIntent().getByteArrayExtra("bitmap");
//        setImag(getPicFromBytes(data,null));
//        setHeight(getIntent().getStringExtra("length"));
//        setWidth(getIntent().getStringExtra("width"));
//        setArea(getIntent().getStringExtra("area"));


        setImag(UtilsToService.getInstance().getDownLoadBitmap());
        setWidth(UtilsToService.getInstance().getWidth());
        setHeight(UtilsToService.getInstance().getLength());
        setArea(UtilsToService.getInstance().getArea());
        setcontent(UtilsToService.getInstance().getcontent());
        //========================================
    }
    private void init() {
        mWidth = findViewById(R.id.width);
        mHeight = findViewById(R.id.height);
        mArea = findViewById(R.id.aera);
        mImag = findViewById(R.id.img);
        mcontent=findViewById(R.id.add_content);
    }

    public void setWidth(String width){
        mWidth.setText(width);
    }
    public void setHeight(String height){
        mHeight.setText(height);
    }
    public void setArea(String area){
        mArea.setText(area);
    }

    public void setImag(Bitmap bitmap){
        mImag.setImageBitmap(bitmap);
    }
    public void setcontent(String add){ mcontent.setText(add); }
    //下面的这个方法是将byte数组转化为Bitmap对象的一个方法
    public static Bitmap getPicFromBytes(byte[] bytes, BitmapFactory.Options opts) {

        if (bytes != null)
            if (opts != null)
                return BitmapFactory.decodeByteArray(bytes, 0, bytes.length,  opts);
            else
                return BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
        return null;

    }
}
