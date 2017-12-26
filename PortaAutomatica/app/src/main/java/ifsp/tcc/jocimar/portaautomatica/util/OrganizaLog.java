package ifsp.tcc.jocimar.portaautomatica.util;

import java.util.ArrayList;
import java.util.List;

import ifsp.tcc.jocimar.portaautomatica.DAO.SalvaLog;
import ifsp.tcc.jocimar.portaautomatica.entidade.Log;

/**
 * Created by paulo on 13/11/17.
 */

public class OrganizaLog {


    public static List<String> linhas = new ArrayList<>();

    public static void trasformaStringEmLogs(String logs) {
        linhas = vetorTorList(logs.split("#"));
        for (String linha : linhas) {
            String logEmVetorDeString[] = linha.split(",");

            //if para ignorar possivel sujeira que venha na ultima linha com caracteres ???
            if (linha.length() < 48) {
                Log logParaSalvar = new Log();
                logParaSalvar.setDataHora(logEmVetorDeString[0]);
                logParaSalvar.setCodigoTag(logEmVetorDeString[1]);
                logParaSalvar.setPermitido(logEmVetorDeString[2]);

                //Salvado o log dentro da lista
                SalvaLog.addLog(logParaSalvar);
            }

        }
    }

    /**
     * Converte um vetor de String em uma lista de String
     *
     * @param string
     * @return Lista de String
     */
    private static List<String> vetorTorList(String[] string) {
        List<String> listaConvertidaDoVetor = new ArrayList<>();
        for (int i = 0; i < string.length; i++) {
            listaConvertidaDoVetor.add(string[i]);
        }
        return listaConvertidaDoVetor;
    }
}
