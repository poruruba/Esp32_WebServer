'use strict';

//const vConsole = new VConsole();
//window.datgui = new dat.GUI();

const http_url = "/post";

var vue_options = {
    el: "#top",
    mixins: [mixins_bootstrap],
    data: {
        target_width: 320,
        target_height: 240,
        quality: 50,
    },
    computed: {
    },
    methods: {
        image_click: function (e) {
            e.target.value = '';
        },
        image_open: function (e) {
            this.image_open_file(e.target.files[0]);
        },
        image_open_file: function (file) {
            if (!file.type.startsWith('image/')) {
                alert('画像ファイルではありません。');
                return;
            }

            var reader = new FileReader();
            reader.onload = (theFile) => {
                this.image_image = new Image();
                this.image_image.onload = () => {
                    this.image_scale_change();
                };
                this.image_image.src = reader.result;
            };
            reader.readAsDataURL(file);
        },
        image_scale_change: function () {
            var image = this.image_image;
            var scale = image.width / this.target_width;
            if (scale < image.height / this.target_height )
                scale = image.height / this.target_height;

            var canvas = document.querySelector('#canvas_image');
            var context = canvas.getContext('2d');

            context.drawImage(image, 0, 0, image.width, image.height, 0, 0, Math.round(image.width / scale), Math.round(image.height / scale ));
            if( !confirm("画像を転送しますか？") )
                return;
            
            this.image_transfer();
        },
        image_transfer: async function(){
            try{
                var canvas = document.querySelector('#canvas_image');
                var dataURL = canvas.toDataURL('image/jpeg', this.quality / 100);
                var index = dataURL.indexOf(',');
                if (index >= 0) {
                    var params = {
                        param: dataURL.slice(index + 1)
                    };
                    await do_post(http_url, params);
                    alert("転送しました。");
                }
            }catch(error){
                alert(error);
            }
        }
    },
    created: function(){
    },
    mounted: function(){
        proc_load();

    }
};
vue_add_data(vue_options, { progress_title: '' }); // for progress-dialog
vue_add_global_components(components_bootstrap);

/* add additional components */

window.vue = new Vue( vue_options );

function do_post(url, body) {
    const headers = new Headers({ "Content-Type": "application/json" });

    return fetch(url, {
        method: 'POST',
        body: JSON.stringify(body),
        headers: headers
    })
        .then((response) => {
            if (!response.ok)
                throw 'status is not 200';
            return response.json();
            //    return response.text();
            //    return response.blob();
            //    return response.arrayBuffer();
        });
}
