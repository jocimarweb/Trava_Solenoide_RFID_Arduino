package ifsp.tcc.jocimar.portaautomatica;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import ifsp.tcc.jocimar.portaautomatica.DAO.SalvaLog;
import ifsp.tcc.jocimar.portaautomatica.util.OrganizaLog;

public class MainActivity extends Activity {

    Button conectar;
    Button desconectar;
    Button obterLog;
    Button visualizar;

    private BluetoothDevice dispositivoBlutoothRemoto;
    private BluetoothAdapter meuBluetoothAdapter = null;
    private BluetoothSocket bluetoothSocket = null;
    private static final int CODIGO_PARA_ATIVACAO_DO_BLUETOOTH = 1;

    private static final String endereco_MAC_do_bluetooth_remoto = "98:D3:32:20:4B:0E";
    private static final UUID MEU_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private InputStream inputStream = null;
    private OutputStream outputStream = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        fazerConexoes();
        verificarCondicaoDoBluetooth();
    }

    public void fazerConexoes() {
        conectar = (Button) findViewById(R.id.btnConectar);
        desconectar = (Button) findViewById(R.id.btnDesconectar);
        obterLog = (Button) findViewById(R.id.btnLog);
        visualizar = (Button) findViewById(R.id.btnVisualizar);

        //define as ações dos botões
        conectar.setOnClickListener(new Conectar());
        desconectar.setOnClickListener(new Desconectar());
        obterLog.setOnClickListener(new ReceberLog());
        visualizar.setOnClickListener(new VisualizarLog());
    }

    public void verificarCondicaoDoBluetooth() {
        meuBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        //Verifica se o celular tem bluetooth
        if (meuBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(), "Dispositivo não possui adaptador Bluetooth", Toast.LENGTH_LONG).show();

            //Finaliza aplicação
            finish();
        } else {
            //Verifica se o bluetooth está desligado. Se sim pede permissão para ligar
            if (!meuBluetoothAdapter.isEnabled()) {
                Intent novoIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(novoIntent, CODIGO_PARA_ATIVACAO_DO_BLUETOOTH);
            }
        }
    }

    public class Conectar implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            if (BluetoothAdapter.checkBluetoothAddress(endereco_MAC_do_bluetooth_remoto)) {
                dispositivoBlutoothRemoto = meuBluetoothAdapter.getRemoteDevice(endereco_MAC_do_bluetooth_remoto);
            } else {
                Toast.makeText(getApplicationContext(), "Endereço MAC do dispositivo móvel não é válido", Toast.LENGTH_SHORT).show();
            }

            try {
                bluetoothSocket = dispositivoBlutoothRemoto.createInsecureRfcommSocketToServiceRecord(MEU_UUID);
                bluetoothSocket.connect();
                Toast.makeText(getApplicationContext(), "Conectado", Toast.LENGTH_LONG).show();
                SalvaLog.zeraLog();
                obterLog.setEnabled(true);
                desconectar.setEnabled(true);
            } catch (Exception e) {
                Log.e("ERRO AO CONECTAR", "Erro:" + e.getMessage());
                Toast.makeText(getApplicationContext(), "Conexão não foi estabelecida", Toast.LENGTH_LONG);
            }
        }
    }

    public class Desconectar implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            if (bluetoothSocket != null) {
                try {
                    bluetoothSocket.close();
                    bluetoothSocket = null;
                    Toast.makeText(getApplicationContext(), "Conexão encerada", Toast.LENGTH_LONG).show();
                    desconectar.setEnabled(false);
                } catch (IOException e) {
                    Log.e("ERRO AO DESCONECTAR", "O erro foi:" + e.getMessage());
                    Toast.makeText(getApplicationContext(), "Erro - A conexão permanece estabelecida", Toast.LENGTH_LONG).show();
                }
            } else {
                Toast.makeText(getApplicationContext(), "Nao há nenhuma conexão estabelecida para ser desconectada", Toast.LENGTH_LONG).show();
            }
        }
    }

    public class ReceberLog implements View.OnClickListener {
        private void sendData(String message) {
            byte[] msgBuffer = message.getBytes();

            try {
                outputStream.write(msgBuffer);
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "Erro - Ao envaiar dados", Toast.LENGTH_LONG).show();
            }
        }


        @Override
        public void onClick(View view) {
            if (bluetoothSocket != null) {
                //string para concatenar caracter sem usar +
                StringBuilder mensagem = new StringBuilder();

                try {
                    outputStream = bluetoothSocket.getOutputStream();
                    //o caractere 0 é o codigo para o arduino responder com o log completo
                    sendData("0");
                    SystemClock.sleep(2000);

                    inputStream = bluetoothSocket.getInputStream();

                    byte[] msgBuffer = new byte[1024];
                    inputStream.read(msgBuffer);

                    //A classe organiza vai fazer a divisão das strings em cada linha pelo caracter '#'
                    //e a divisão de cada campo do log pelo caracter ',' e salvar dentro do vetor na classe SalvaLog
                    OrganizaLog.trasformaStringEmLogs(new String(msgBuffer));

                    //coloca na tela a string do log
                    mensagem.append(SalvaLog.listarLogString());
                    //desabilita o botão Capturar Log
                    obterLog.setEnabled(false);

                } catch (IOException e) {
                    Log.e("ERROR", "Erro:" + e.getMessage());
                    Toast.makeText(getApplicationContext(), "Mensagem não recebida", Toast.LENGTH_LONG).show();
                }

            }
        }
    }

    public class VisualizarLog implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            Intent intent = new Intent(getApplicationContext(), Lista.class);
            startActivity(intent);
        }
    }
}