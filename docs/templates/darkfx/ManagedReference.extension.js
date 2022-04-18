/**
 * This method will be called at the end of exports.transform in ManagedReference.html.primary.js
 */
 exports.postTransform = function (model) {
   model.assemblies = ["Chroma.Mono"]
    return model;
  }