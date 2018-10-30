package com.tina.pushstream;

import android.Manifest;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

import com.tbruyelle.rxpermissions2.RxPermissions;
import com.tina.pushstream.live.LivePusher;

public class MainActivity extends AppCompatActivity {


    private LivePusher livePusher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        permission();

        livePusher = new LivePusher(this, 800, 480, 800_000, 10, Camera.CameraInfo.CAMERA_FACING_BACK);
        //  设置摄像头预览的界面
        livePusher.setPreviewDisplay(surfaceView.getHolder());
    }

    public void switchCamera(View view) {
        livePusher.switchCamera();
    }

    public void startLive(View view) {
        livePusher.startLive("rtmp://47.75.90.219/myapp/mystream");
    }

    public void stopLive(View view) {
        livePusher.stopLive();
    }

    private void permission() {
        RxPermissions rxPermissions = new RxPermissions(this);
        rxPermissions.request(Manifest.permission.CAMERA,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                .subscribe(granted -> {
                    if (granted) {
                        Toast.makeText(getApplication(), "用户已经同意该权限", Toast.LENGTH_SHORT).show();
                        // All requested permissions are granted
                    } else {
                        // At least one permission is denied
                        Toast.makeText(getApplication(), "用户拒绝了该权限", Toast.LENGTH_SHORT).show();
                    }
                });

        rxPermissions
                .requestEach(Manifest.permission.CAMERA,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE)
                .subscribe(permission -> { // will emit 2 Permission objects
                    if (permission.granted) {
                        Toast.makeText(getApplication(), "用户已经同意该权限", Toast.LENGTH_SHORT).show();
                    } else if (permission.shouldShowRequestPermissionRationale) {
                        // 用户拒绝了该权限，没有选中『不再询问』（Never ask again）,那么下次再次启动时，还会提示请求权限的对话框
                        Toast.makeText(getApplication(), "用户拒绝了该权限", Toast.LENGTH_SHORT).show();
                    } else {
                        // 用户拒绝了该权限，并且选中『不再询问』，提醒用户手动打开权限
                        Toast.makeText(getApplication(), "权限被拒绝，请在设置里面开启相应权限，若无相应权限会影响使用", Toast.LENGTH_SHORT).show();
                    }
                });
    }

}