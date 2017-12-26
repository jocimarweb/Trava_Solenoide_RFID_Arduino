package ifsp.tcc.jocimar.portaautomatica;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.List;

import ifsp.tcc.jocimar.portaautomatica.DAO.SalvaLog;
import ifsp.tcc.jocimar.portaautomatica.entidade.Log;

public class Lista extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lista);

        List<Log> logList = SalvaLog.listarLogObjetos();

        ListView ltvLista = (ListView) findViewById(R.id.ltvLista);

        ArrayAdapter<Log> adapter = new ArrayAdapter<Log>(this,
                android.R.layout.simple_list_item_1, logList);

        ltvLista.setAdapter(adapter);
    }
}
