package ifsp.tcc.jocimar.portaautomatica.DAO;

import java.util.ArrayList;
import java.util.List;

import ifsp.tcc.jocimar.portaautomatica.entidade.Log;

/**
 * Created by Jocimar on 13/11/17.
 */

public class SalvaLog {

    //armazena os logs
    private static List<Log> logsCapturados = new ArrayList<>();

    public static void addLog(Log log){
        if(logsCapturados!=null){
            logsCapturados.add(log);
        }
    }

    public static void zeraLog(){
        logsCapturados = null;
        logsCapturados = new ArrayList<>();
    }

    /**
     * Lista todos os logs como um List<Log>
     * @return List<Log>
     */
    public static List<Log> listarLogObjetos(){
        return logsCapturados;
    }

    /**
     * Lista todos os logs convertidos em uma String
     * @return String
     */
    public static String listarLogString(){
        StringBuilder logsComoString = new StringBuilder();
        for (Log log: logsCapturados) {
            logsComoString.append(log.getDataHora()).append(" ");
            logsComoString.append(log.getCodigoTag()).append(" ");
            logsComoString.append(log.getPermitido());
            logsComoString.append("\n");
        }

        return logsComoString.toString();
    }

}
