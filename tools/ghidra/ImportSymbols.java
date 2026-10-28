// Import the project's symbol names: labels for functions and RAM objects, with the
// C declaration as a plate comment. Reads the TSV that tools/symexport.py writes:
//   python3 tools/symexport.py --ghidra build/symbols/ghidra.tsv
//   ... -process baserom.gba -noanalysis -scriptPath tools/ghidra \
//       -postScript ImportSymbols.java build/symbols/ghidra.tsv [--skip-placeholders]
// Columns: addr name kind size mode confidence comment
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.lang.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.math.BigInteger;

public class ImportSymbols extends GhidraScript {
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String path = args.length > 0 ? args[0] : "build/symbols/ghidra.tsv";
        boolean skip = args.length > 1 && args[1].equals("--skip-placeholders");
        Listing lst = currentProgram.getListing();
        Register tmode = currentProgram.getProgramContext().getRegister("TMode");
        int named = 0, made = 0, comments = 0;
        try (BufferedReader in = new BufferedReader(new FileReader(path))) {
            String line = in.readLine();               // header
            while ((line = in.readLine()) != null) {
                String[] c = line.split("\t", -1);
                if (c.length < 7) continue;
                String name = c[1], kind = c[2], mode = c[4], comment = c[6];
                if (skip && name.matches("(sub|gUnk|gUnknown|unk)_[0-9A-Fa-f]{8}")) continue;
                Address ad = toAddr(Long.parseLong(c[0], 16));
                if (ad == null || !currentProgram.getMemory().contains(ad)) continue;
                if (kind.equals("function")) {
                    if (mode.equals("thumb"))
                        currentProgram.getProgramContext().setValue(tmode, ad, ad, BigInteger.ONE);
                    if (lst.getFunctionAt(ad) == null && getFunctionContaining(ad) == null) {
                        disassemble(ad);
                        if (createFunction(ad, name) != null) made++;
                    }
                }
                try {
                    Function f = lst.getFunctionAt(ad);
                    if (f != null) f.setName(name, SourceType.USER_DEFINED);
                    else createLabel(ad, name, true, SourceType.USER_DEFINED);
                    named++;
                } catch (Exception e) {
                    printerr(c[0] + " " + name + ": " + e.getMessage());
                }
                if (!comment.isEmpty() && kind.startsWith("ram")) {
                    lst.setComment(ad, CodeUnit.PLATE_COMMENT, comment);
                    comments++;
                }
            }
        }
        println("ImportSymbols: " + named + " names, " + made + " functions created, " + comments + " comments");
    }
}
