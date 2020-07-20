package com.demo.opencv;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.StrictMode;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.io.File;
import java.io.IOException;

public class ImageSourceActivity extends AppCompatActivity implements View.OnClickListener, UtilsToService.getResultListener {

    private static final String TAG = "ImageSourceActivity";
    //需要的权限数组 读/写/相机
    private static String[] PERMISSIONS_STORAGE = {Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.CAMERA};

    private Button LoadPictureBtn;
    private Button ChosePictureBtn;
    private Button CameraPictureBtn;

    private Button RecognizeBtn;
    private ImageView ImageView01;

    public Bitmap mBitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //跳转相机动态权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            StrictMode.VmPolicy.Builder builder = new StrictMode.VmPolicy.Builder();
            StrictMode.setVmPolicy(builder.build());
        }
        setContentView(R.layout.activity_image_source);
        initView();
        UtilsToService.getInstance().addListener(this);
    }

    private void initView() {
        LoadPictureBtn = findViewById(R.id.loadpicture);
        ChosePictureBtn = findViewById(R.id.imagedata);
        CameraPictureBtn = findViewById(R.id.camerapick);
        RecognizeBtn = findViewById(R.id.recognize);
        RecognizeBtn.setOnClickListener(this);
        LoadPictureBtn.setOnClickListener(this);
        ChosePictureBtn.setOnClickListener(this);
        CameraPictureBtn.setOnClickListener(this);
        ImageView01 = findViewById(R.id.ImageView);
    }

    /**
     * 检查是否有对应权限
     *
     * @param activity   上下文
     * @param permission 要检查的权限
     * @return 结果标识
     */
    public int verifyPermissions(Activity activity, java.lang.String permission) {
        int Permission = ActivityCompat.checkSelfPermission(activity, permission);
        if (Permission == PackageManager.PERMISSION_GRANTED) {
            Log.e(TAG, "已经同意权限");
            return 1;
        } else {
            Log.e(TAG, "没有同意权限");
            return 0;
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.loadpicture: {
                toPicture();
                break;
            }
            case R.id.imagedata: {
                break;
            }
            case R.id.camerapick: {
                //检查是否已经获得相机的权限
                if (verifyPermissions(ImageSourceActivity.this, PERMISSIONS_STORAGE[2]) == 0) {
                    Log.e(TAG, "提示是否要授权");
                    ActivityCompat.requestPermissions(ImageSourceActivity.this, PERMISSIONS_STORAGE, 3);
                } else {
                    //已经有权限
                    toCamera();  //打开相机
                }

                break;
            }

            case R.id.recognize: {
//                //传给算法mBitmap
//                //算法给我4个参数
//                double length=10;//测试
//                double width=10;
//                double area=1;
//                Bitmap image=mBitmap;
//
//                JumPShowImageActivity(length,width,area,image);
                if(mBitmap!=null){
                    UtilsToService.getInstance().setUpLoadBitmap(mBitmap);
                }
                break;
            }
            default:
                break;
        }
    }

    private File tempFile = null;   //新建一个 File 文件（用于相机拿数据）



    //获取 相机 或者 图库 返回的图片
    @SuppressLint("MissingSuperCall")
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        //判断返回码不等于0
        if (requestCode != RESULT_CANCELED) {    //RESULT_CANCELED = 0(也可以直接写“if (requestCode != 0 )”)
            //读取返回码
            switch (requestCode) {
                case 100:   //相册返回的数据（相册的返回码）
                    Log.e(TAG, "相册");
                    if (data == null) {
                        return;
                    }
                    Uri uri01 = data.getData();
                    try {
                        Bitmap bitmap = BitmapFactory.decodeStream(getContentResolver().openInputStream(uri01));
                        ImageView01.setImageBitmap(bitmap);
                        mBitmap = bitmap;
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                    break;
                case 101:  //相机返回的数据（相机的返回码）
                    Log.e(TAG, "相机");
                    try {
                        tempFile = new File(Environment.getExternalStorageDirectory(), "fileImg.jpg");  //相机取图片数据文件
                        Uri uri02 = Uri.fromFile(tempFile);   //图片文件
                        Log.e(TAG, "相机" + tempFile);
                        Bitmap bitmap = BitmapFactory.decodeStream(getContentResolver().openInputStream(uri02));
                        ImageView01.setImageBitmap(bitmap);
                        mBitmap = bitmap;
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    break;
            }
        }
    }

    //跳转相册
    private void toPicture() {
        Intent intent = new Intent(Intent.ACTION_PICK);  //跳转到 ACTION_IMAGE_CAPTURE
        intent.setType("image/*");
        startActivityForResult(intent, 100);
        Log.e(TAG, "跳转相册成功");
    }

    //跳转相机
    private void toCamera() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);  //跳转到 ACTION_IMAGE_CAPTURE
        //判断内存卡是否可用，可用的话就进行存储
        //putExtra：取值，Uri.fromFile：传一个拍照所得到的文件，fileImg.jpg：文件名
        intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(new File(Environment.getExternalStorageDirectory(), "fileImg.jpg")));
        startActivityForResult(intent, 101); // 101: 相机的返回码参数（随便一个值就行，只要不冲突就好）
        Log.e(TAG, "跳转相机成功");
    }


//    public void JumPShowImageActivity(double x,double y,double z,Bitmap m) {
//        Intent intent = new Intent(this, ShowImageActivity.class);
//        if(m ==null) {
//            return;
//        }
//        //_____________________________if 穿图片，mBitmap=那张处理后的图片
//        ByteArrayOutputStream output = new ByteArrayOutputStream();//初始化一个流对象
//        Bitmap newbit = Bitmap.createScaledBitmap(m,1000,1000,true);
//        newbit.compress(Bitmap.CompressFormat.WEBP, 85, output);//把bitmap100%高质量压缩 到 output对象里
//        byte[] result = output.toByteArray();//转换成功了  result就是一个bit的资源数组
//        //————————————————————if  穿图片的url则上面的不需要，下面这句话改一下
//        intent.putExtra("bitmap",result);
//        //传长、宽、面积
//        String chang =String.valueOf(x);
//        String kuan= String.valueOf(y);
//        String mian=String.valueOf(z);
//        intent.putExtra("length",chang);
//        intent.putExtra("width",kuan);
//        intent.putExtra("area",mian);
//        startActivity(intent);
//    }

    public void JumPShowImageUrlActivity() {
        Intent intent = new Intent(this, ShowImageActivity.class);;
        startActivity(intent);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mBitmap = null;
    }

    @Override
    public void getResult() {
        JumPShowImageUrlActivity();
    }
}
