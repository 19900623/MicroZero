﻿using Senparc.Weixin.Exceptions;
using Senparc.Weixin.MP.Entities;
using Agebull.ZeroNet.ZeroApi;
using Senparc.NeuChar.Entities;
using Senparc.NeuChar.Helpers;
using Agebull.Common.Configuration;
using Microsoft.Extensions.Configuration;

#pragma warning disable CS1591 // 缺少对公共可见类型或成员的 XML 注释
namespace ApiTest
{
    /// <summary>
    /// Weixin服务
    /// </summary>
    public class WeixinController :ApiController
    {
        static WeixinConfig ReadOption()
        {
            var sec = ConfigurationManager.Root.GetSection("SenparcWeixinSetting");
            return sec.Get<WeixinConfig>() ?? new WeixinConfig();
        }
        static WeixinConfig _option;

        private static WeixinConfig Option => _option ?? (_option = ReadOption());
        /// <summary>
        /// 处理文字请求
        /// </summary>
        /// <returns></returns>
        [Route("v1/msg/text")]
        public string OnTextRequest(RequestMessageText requestMessage)
        {
            return defXml;
        }

        /// <summary>
        /// 处理位置请求
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/msg/local")]
        public string OnLocationRequest(RequestMessageLocation requestMessage)
        {
            
            return defXml;
        }
        /// <summary>
        /// 收到视频
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/msg/shortvedio")]
        public string OnShortVideoRequest(RequestMessageShortVideo requestMessage)
        {
            
            return defXml;
        }

        /// <summary>
        /// 处理图片请求
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/msg/image")]
        public string OnImageRequest(RequestMessageImage requestMessage)
        {
            
            return defXml;
        }

        /// <summary>
        /// 处理语音请求
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/msg/voice")]
        public string OnVoiceRequest(RequestMessageVoice requestMessage)
        {
            
            return defXml;
        }

        /// <summary>
        /// 处理视频请求
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/msg/vedio")]
        public string OnVideoRequest(RequestMessageVideo requestMessage)
        {
            
            return defXml;
        }


        /// <summary>
        /// 处理链接消息请求
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/msg/link")]
        public string OnLinkRequest(RequestMessageLink requestMessage)
        {
            
            return defXml;
        }

        [Route("v1/msg/file")]
        public string OnFileRequest(RequestMessageFile requestMessage)
        {
            
            return defXml;
        }

        [Route("v1/msg/def")]
        public string DefaultResponseMessage(IRequestMessageBase requestMessage)
        {
            
            return defXml;
        }

        [Route("v1/msg/unknown")]

        public string OnUnknownTypeRequest(RequestMessageUnknownType requestMessage)
        {
            
            return defXml;
        }

        const string defXml = @"<xml>
  <MsgType><![CDATA[text]]></MsgType>
  <Content><![CDATA[]]></Content>
  <ToUserName><![CDATA[os9oa0kma85FaSRj4gOubhpsx9A8]]></ToUserName>
  <FromUserName><![CDATA[gh_3b59212c8224]]></FromUserName>
  <CreateTime>1542631776</CreateTime>
</xml>";
        /// <summary>
        /// 点击事件
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/click")]
        public string OnEvent_ClickRequest(RequestMessageEvent_Click requestMessage)
        {
            return defXml;
        }

        /// <summary>
        /// 进入事件
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/enter")]
        public string OnEvent_EnterRequest(RequestMessageEvent_Enter requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 位置事件
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/local")]
        public string OnEvent_LocationRequest(RequestMessageEvent_Location requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 通过二维码扫描关注扫描事件
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/scan")]
        public string OnEvent_ScanRequest(RequestMessageEvent_Scan requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 打开网页事件
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/view")]
        public string OnEvent_ViewRequest(RequestMessageEvent_View requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 群发完成事件
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/massSend")]
        public string OnEvent_MassSendJobFinishRequest(RequestMessageEvent_MassSendJobFinish requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 订阅（关注）事件
        /// </summary>
        /// <returns></returns>
        [Route("v1/event/sub")]
        public string OnEvent_SubscribeRequest(RequestMessageEvent_Subscribe requestMessage)
        {
            var msg = ResponseMessageBase.CreateFromRequestMessage<ResponseMessageText>(requestMessage);
            msg.Content = "欢迎光临";
            return msg.ConvertEntityToXmlString();
        }

        /// <summary>
        /// 退订
        /// 实际上用户无法收到非订阅账号的消息，所以这里可以随便写。
        /// unsubscribe事件的意义在于及时删除网站应用中已经记录的OpenID绑定，消除冗余数据。并且关注用户流失的情况。
        /// </summary>
        /// <returns></returns>
        [Route("v1/event/unsub")]
        public string OnEvent_UnsubscribeRequest(RequestMessageEvent_Unsubscribe requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之扫码推事件(scancode_push)
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/scanCode/push")]
        public string OnEvent_ScancodePushRequest(RequestMessageEvent_Scancode_Push requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之扫码推事件且弹出“消息接收中”提示框(scancode_waitmsg)
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/scanCode/wait")]
        public string OnEvent_ScancodeWaitmsgRequest(RequestMessageEvent_Scancode_Waitmsg requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之弹出拍照或者相册发图（pic_photo_or_album）
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/picPhotoOrAlbum")]
        public string OnEvent_PicPhotoOrAlbumRequest(RequestMessageEvent_Pic_Photo_Or_Album requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之弹出系统拍照发图(pic_sysphoto)
        /// 实际测试时发现微信并没有推送RequestMessageEvent_Pic_Sysphoto消息，只能接收到用户在微信中发送的图片消息。
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/picSysphoto")]
        public string OnEvent_PicSysphotoRequest(RequestMessageEvent_Pic_Sysphoto requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之弹出微信相册发图器(pic_weixin)
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/pic/weixin")]
        public string OnEvent_PicWeixinRequest(RequestMessageEvent_Pic_Weixin requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之弹出地理位置选择器（location_select）
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/loc/select")]
        public string OnEvent_LocationSelectRequest(RequestMessageEvent_Location_Select requestMessage)
        {

            return defXml;
        }

        /// <summary>
        /// 事件之发送模板消息返回结果
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/tmp/sended")]
        public string OnEvent_TemplateSendJobFinishRequest(RequestMessageEvent_TemplateSendJobFinish requestMessage)
        {
            switch (requestMessage.Status)
            {
                case "success":
                    //发送成功
                    break;
                case "failed:user block":
                    //送达由于用户拒收（用户设置拒绝接收公众号消息）而失败
                    break;
                case "failed: system failed":
                    //送达由于其他原因失败
                    break;
                default:
                    throw new WeixinException("未知模板消息状态：" + requestMessage.Status);
            }


            return defXml;
        }

        #region 微信认证事件推送
        /// <summary>
        /// 微信认证事件推送
        /// </summary>
        /// <param name="requestMessage"></param>
        /// <returns></returns>
        [Route("v1/event/qualificationVerifySuccess")]
        public string OnEvent_QualificationVerifySuccessRequest(RequestMessageEvent_QualificationVerifySuccess requestMessage)
        {
            return defXml;
        }

        #endregion
    }
}

#pragma warning restore CS1591 // 缺少对公共可见类型或成员的 XML 注释