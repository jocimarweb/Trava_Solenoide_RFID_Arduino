package ifsp.tcc.jocimar.portaautomatica.entidade;

/**
 * Created by Jocimar on 13/11/17.
 */

public class Log {

    private String dataHora;
    private String codigoTag;
    private String permitido;

    public String getDataHora() {
        return dataHora;
    }

    public void setDataHora(String dataHora) {
        this.dataHora = dataHora;
    }

    public String getCodigoTag() {
        return codigoTag;
    }

    public void setCodigoTag(String codigoTag) {
        this.codigoTag = codigoTag;
    }

    public String getPermitido() {
        return permitido;
    }

    public void setPermitido(String permitido) {
        this.permitido = permitido;
    }

    @Override
    public String toString() {
        return  "Data/Hora: " + dataHora + "\n" +
                "CodigoTag/Porta: "  + codigoTag + "\n" +
                "Status: " + permitido;
    }
}
