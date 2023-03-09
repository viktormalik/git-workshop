const options = {
    template: GitgraphJS.templateExtend("metro", {
        colors: ["#ee0000", "#008fb5", "#979797"],
        commit: {
            message: {
                displayAuthor: false,
                displayEmail: false,
                displayBranch: true,
                displayHash: false,
                font: "normal 14pt Overpass"
            }
        },
        branch: {
            label: {
                font: "normal 14pt Overpass"
            }
        }
    }),
};
