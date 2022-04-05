import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';
import * as yaml from 'yaml';

/**
 * CodelensProvider
 */
export class CodelensProvider implements vscode.CodeLensProvider {

    private codeLenses: vscode.CodeLens[] = [];
    private regex: RegExp;
    private guid_regex: RegExp;
    private _onDidChangeCodeLenses: vscode.EventEmitter<void> = new vscode.EventEmitter<void>();
    public readonly onDidChangeCodeLenses: vscode.Event<void> = this._onDidChangeCodeLenses.event;

    constructor() {
        this.regex = /(.+)/g;
        this.guid_regex = /(\{){0,1}[0-9a-fA-F]{8}\-{0,1}[0-9a-fA-F]{4}\-{0,1}[0-9a-fA-F]{4}\-{0,1}[0-9a-fA-F]{4}\-{0,1}[0-9a-fA-F]{12}(\}){0,1}/g;
        vscode.workspace.onDidChangeConfiguration((_) => {
            this._onDidChangeCodeLenses.fire();
        });
    }

    public provideCodeLenses(document: vscode.TextDocument, token: vscode.CancellationToken): vscode.CodeLens[] | Thenable<vscode.CodeLens[]> {

        if (vscode.workspace.getConfiguration("polychrome").get("enableCodeLens", true)) {
            this.codeLenses = [];
            const regex = new RegExp(this.regex);
            const text = document.getText();
            let matches;
            while ((matches = regex.exec(text)) !== null) {
                const line = document.lineAt(document.positionAt(matches.index).line);
                const indexOf = line.text.indexOf(matches[0]);
                const position = new vscode.Position(line.lineNumber, indexOf);
                const gregex = new RegExp(this.guid_regex);
                const range = document.getWordRangeAtPosition(position, new RegExp(this.regex));
                let guid_result = gregex.exec(line.text);
                let found_guid = guid_result ? guid_result[0] : null;
                if (range && found_guid !== null) {
                    let workspaces = vscode.workspace.workspaceFolders
                    if (workspaces !== undefined) {
                        for (let w of workspaces) {
                            let asset_path = path.join(w.uri.fsPath, "Assets.yaml");
                            let data = fs.readFileSync(asset_path, 'utf8');
                            if (data !== undefined){
                                let yaml_result = yaml.parse(data);
                                for (const guid in yaml_result) {
                                    if (guid === found_guid){
                                        const item_path = yaml_result[guid]['Path'];
                                        let cmd = {
                                            title: item_path,
                                            tooltip: item_path + "; " + guid,
                                            command: "polychrome.codelensAction",
                                            arguments: [item_path, w]
                                        };
                                        this.codeLenses.push(new vscode.CodeLens(range, cmd))
                                    }

                                }
                            }
                        }
                            
                    }
                    
                }
            }
            return this.codeLenses;
        }
        return [];
    }
}