"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.CodeCompletionItemProvider = void 0;
const vscode = require("vscode");
const fs = require("fs");
const path = require("path");
const yaml = require("yaml");
class CodeCompletionItemProvider {
    constructor() {
        this.completions = [];
    }
    provideCompletionItems(document, position, token) {
        this.completions = [];
        let workspaces = vscode.workspace.workspaceFolders;
        if (workspaces !== undefined) {
            for (let workspace of workspaces) {
                let asset_path = path.join(workspace.uri.fsPath, "Assets.yaml");
                let data = fs.readFileSync(asset_path, 'utf8');
                if (data !== undefined) {
                    let yaml_result = yaml.parse(data);
                    for (const guid in yaml_result) {
                        const path = yaml_result[guid]['Path'];
                        const commitCharacterCompletion = new vscode.CompletionItem(path);
                        commitCharacterCompletion.documentation = new vscode.MarkdownString("\"" + guid + "\"");
                        commitCharacterCompletion.insertText = "\"" + guid + "\"";
                        this.completions.push(commitCharacterCompletion);
                    }
                }
            }
        }
        return this.completions;
    }
}
exports.CodeCompletionItemProvider = CodeCompletionItemProvider;
//# sourceMappingURL=CodeCompletionProvider.js.map