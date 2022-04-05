"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.CodelensProvider = void 0;
const vscode = require("vscode");
const fs = require("fs");
const path = require("path");
const yaml = require("yaml");
/**
 * CodelensProvider
 */
class CodelensProvider {
    constructor() {
        this.codeLenses = [];
        this._onDidChangeCodeLenses = new vscode.EventEmitter();
        this.onDidChangeCodeLenses = this._onDidChangeCodeLenses.event;
        this.regex = /(.+)/g;
        this.guid_regex = /(\{){0,1}[0-9a-fA-F]{8}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{12}(\}){0,1}/g;
        vscode.workspace.onDidChangeConfiguration((_) => {
            this._onDidChangeCodeLenses.fire();
        });
    }
    provideCodeLenses(document, token) {
        if (vscode.workspace.getConfiguration("polychrome").get("enableCodeLens", true)) {
            this.codeLenses = [];
            const regex = new RegExp(this.regex);
            const text = document.getText();
            let matches;
            while ((matches = regex.exec(text)) !== null) {
                const line = document.lineAt(document.positionAt(matches.index).line);
                const indexOf = line.text.indexOf(matches[0]);
                const position = new vscode.Position(line.lineNumber, indexOf);
                const range = document.getWordRangeAtPosition(position, new RegExp(this.regex));
                let guid_result = this.guid_regex.exec(line.text);
                let found_guid = guid_result ? guid_result[0] : null;
                if (range && found_guid !== null) {
                    let workspaces = vscode.workspace.workspaceFolders;
                    if (workspaces !== undefined) {
                        for (let workspace of workspaces) {
                            let asset_path = path.join(workspace.uri.fsPath, "Assets.yaml");
                            let data = fs.readFileSync(asset_path, 'utf8');
                            if (data !== undefined) {
                                let yaml_result = yaml.parse(data);
                                for (const guid in yaml_result) {
                                    if (guid === found_guid) {
                                        let path = yaml_result[guid]['Path'];
                                        let cmd = {
                                            title: path,
                                            tooltip: "Path: " + path + ", GUID:" + guid,
                                            command: "polychrome.codelensAction",
                                            arguments: [path, false]
                                        };
                                        this.codeLenses.push(new vscode.CodeLens(range, cmd));
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
exports.CodelensProvider = CodelensProvider;
//# sourceMappingURL=CodelensProvider.js.map